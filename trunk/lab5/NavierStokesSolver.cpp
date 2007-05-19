#include "NavierStokesSolver.h"
#include "LevelSetGrid.h"

const float NavierStokesSolver::mG = 10;

NavierStokesSolver::NavierStokesSolver()
	{
	mCurrentVolume = 0.0;
	mLargestDT = 0.0;
	mTargetVolume = 0.0;
	}

NavierStokesSolver::~NavierStokesSolver()
	{

	}

float NavierStokesSolver::getTimestep(float alpha)
	{
	return mLargestDT*alpha;
	}

void NavierStokesSolver::solve(std::vector<Geometry*>& geometryList, float dt)
	{
	printf("DT: %f\n", dt);

	// Find any fluid level sets..
	for (unsigned int i = 0; i < geometryList.size(); i++)
		{
		if (typeid(*geometryList[i]) == typeid(VolumeLevelSet))
			{
			VolumeLevelSet * ls = dynamic_cast<VolumeLevelSet *>(geometryList[i]);
			mCellSize = ls->getDx();

			if (dt == 0){
				dt = mCellSize;
				}

			// Calculate volume of fluid
			mCurrentVolume = calculateVolume(ls);
			printf("Current volume: %e m^3\n", mCurrentVolume);
			if (mTargetVolume == 0.0){
				mTargetVolume = mCurrentVolume;
				}

			// Crate new fields to work with
			Volume<Vector3<float> >* velocityField = ls->getVelocityField();

			mDimX = velocityField->getDimX();
			mDimY = velocityField->getDimY();
			mDimZ = velocityField->getDimZ();

			mCurrentField = new Volume<Vector3<float> >(mDimX, mDimY, mDimZ, Vector3<float>(0.0f));
			mOldField = new Volume<Vector3<float> >(mDimX, mDimY, mDimZ, Vector3<float>(0.0f));
			mSolidMask = new Volume<bool>(mDimX, mDimY, mDimZ, false);
			mPressureField = new Volume<float>(mDimX, mDimY, mDimZ, 0.0f);
			mRHSField = new Volume<float>(mDimX, mDimY, mDimZ, 0.0f);


			// Copy incoming vector field to working field...
			*mOldField = *velocityField;

			// Semi lagrangian self advection
			selfAdvect(ls, dt);

			// Add forces
			addForces(ls, dt);

			// Enforce dirichlet boundary condition
			enforceDirichletBoundaryCondition(geometryList, mOldField);

			// Perform projection
			//-------------------
			calculateRHS(ls, dt);  //Get the right hand side of the Velocity Extension
			buildMatrix(ls);
			solvePoissonEquation(ls, dt);
			velocityFieldCorrection(ls, dt);

			// Once again enforce dirichlet boundary condition
			// The pressure correction will break the dirichlet boundary condition
			// so this has to be done
			enforceDirichletBoundaryCondition(geometryList, mOldField);

			// Calculate new timestep to satisfy CFL condition
			mLargestDT = 0.0;
			calculateNewTimestep(geometryList);

			// Write back the data...
			*(ls->getVelocityField()) = *mOldField;

			// Build the velocity field used for advecting the level set
			ls->buildAdvectionField();

			// Compute Energy
			static float potentialEnergy, kineticEnergy;
			calculateEnergy(ls, potentialEnergy, kineticEnergy);

			// Display energy
			printf("Current Potential Energy: %f\n", potentialEnergy);
			printf("Current Kinetic Energy: %f\n", kineticEnergy);
			printf("Total Energy: %f\n", kineticEnergy + potentialEnergy);

			delete mCurrentField;
			delete mOldField;
			delete mSolidMask;
			delete mPressureField;
			delete mRHSField;
			}
		}
	}

void NavierStokesSolver::swapFields()
	{
	Volume<Vector3<float> >* temp = mOldField;
	mOldField = mCurrentField;
	mCurrentField = temp;
	}

void NavierStokesSolver::selfAdvect(VolumeLevelSet* ls, float dt)
	{
	// Perform semi-lagrangian advection of the velocity field

	const VolumeLevelSet::VolumeMask& volumeMask = ls->getVolumeMask();
	for (unsigned int p = 0; p < volumeMask.size(); p++)
		{
		const Vector3<int>& pos = volumeMask[p];
		int i = pos.x();
		int j = pos.y();
		int k = pos.z();

		Vector3<float> newV = mSemiLagrangianIntegrator.integrate(pos, *mOldField, dt);
		mCurrentField->setValue(i,j,k,newV);
		}

	std::swap(mCurrentField, mOldField);;
	}

void NavierStokesSolver::addForces(VolumeLevelSet* ls, float dt)
	{
	static const Vector3<float> gForce(0,-mG,0);

	// Add gravity force..
	const VolumeLevelSet::VolumeMask& volumeMask = ls->getVolumeMask();
	for (unsigned int p = 0; p < volumeMask.size(); p++)
		{
		const Vector3<int>& pos = volumeMask[p];
		int i = pos.x();
		int j = pos.y();
		int k = pos.z();

		Vector3<float> newV = mEulerIntegrator.integrate(mOldField->getValue(i,j,k), gForce, dt);

		mCurrentField->setValue(i,j,k,newV);
		}

	std::swap(mCurrentField, mOldField);;
	}

bool NavierStokesSolver::isSolid(int i, int j, int k, std::vector<LevelSet*>& solids)
	{
	for (unsigned int count = 0; count < solids.size(); count++){
		if (solids[count]->getValue(i,j,k) <= 0.0){
			return true;
			}
		}

	return false;
	}

void NavierStokesSolver::enforceDirichletBoundaryCondition(std::vector<Geometry*>& geometryList, Volume<Vector3<float> >* field)
	{
	//Build list of solids in scene
	std::vector<LevelSet*> solids;
	VolumeLevelSet* fluid = NULL;
	for (unsigned int i = 0; i < geometryList.size(); i++){
		if (typeid(*geometryList[i]) == typeid(VolumeLevelSet))
			{
			fluid = dynamic_cast<VolumeLevelSet *>(geometryList[i]);
			}
		else{
			if (typeid(*geometryList[i]) == typeid(LevelSet)){
				solids.push_back(dynamic_cast<LevelSet *>(geometryList[i]));
				}
			}
		}

	// Solid intervals

	// Fluid intervals

	assert(solids.size() < geometryList.size());

	const VolumeLevelSet::VolumeMask& volumeMask = fluid->getVolumeMask();
	for (unsigned int p = 0; p < volumeMask.size(); p++)
		{
		const Vector3<int>& pos = volumeMask[p];
		int i = pos.x();
		int j = pos.y();
		int k = pos.z();


		bool solidInXDirection = false;
		bool solidInYDirection = false;
		bool solidInZDirection = false;

		// Check collision against solids..
		if (isSolid(i+1,j,k, solids)){
			mSolidMask->setValue(i+1,j,k,true);
			solidInXDirection = true;
			}
		if (isSolid(i-1,j,k, solids)){
			mSolidMask->setValue(i-1,j,k,true);
			solidInXDirection = true;
			}

		if (isSolid(i,j+1,k, solids)){
			mSolidMask->setValue(i,j+1,k,true);
			solidInYDirection = true;
			}
		if (isSolid(i,j-1,k, solids)){
			mSolidMask->setValue(i,j-1,k,true);
			solidInYDirection = true;
			}

		if (isSolid(i,j,k+1, solids)){
			mSolidMask->setValue(i,j,k+1,true);
			solidInZDirection = true;
			}
		if (isSolid(i,j,k-1, solids)){
			mSolidMask->setValue(i,j,k-1,true);
			solidInZDirection = true;
			}

		// If we are inside the fluid
		if (fluid->getValue(i,j,k) <= fluid->getInsideConstant()){
			Vector3<float> v = field->getValue(i,j,k);

			if (solidInXDirection)
				{
				v.x() = 0;
				}
			if (solidInYDirection)
				{
				v.y() = 0;
				}
			if (solidInZDirection)
				{
				v.z() = 0;
				}

			field->setValue(i,j,k, v);
			}
		}
	}

void NavierStokesSolver::calculateRHS(VolumeLevelSet* ls, float dt)
	{
	const float dx = mCellSize;
	const VolumeLevelSet::InsideMask& insideMask = ls->getInsideMask();


	for (unsigned int p = 0; p < insideMask.size(); p++){
		const Vector3<int>& pos = insideMask[p];
		int i = pos.x();
		int j = pos.y();
		int k = pos.z();

		// Use central differencing to calculate RHS
		float RHS = mOldField->getValue(i+1,j,k).x() - mOldField->getValue(i-1,j,k).x() +
			mOldField->getValue(i,j+1,k).y() - mOldField->getValue(i,j-1,k).y() +
			mOldField->getValue(i,j,k+1).z() - mOldField->getValue(i,j,k-1).z();

		RHS *= 1.0/2.0/dx;

		mRHSField->setValue(i,j,k, RHS);
		}
	}


void NavierStokesSolver::processVoxel(int i, int j, int k, float& element, float& centerElement, VolumeLevelSet* ls)
	{
	//printf("Build poisson matrix not yet implemented!\n");

	//element = 1.0f;

	////If outside AND solid
	//if( (ls->getValue(i,j,k) > ls->getInsideConstant()) && (mSolidMask->getValue(i,j,k)) )
	//	{
	//	// Voxel is inside a solid
	//	element = 0.0f;
	//	centerElement++;
	//	}

	if (ls->getValue(i,j,k) <= ls->getInsideConstant())
		{
		// Voxel is inside the fluid...
		element = 1.0f;
		}
	else{
		if (mSolidMask->getValue(i,j,k))
			{
			// Voxel is inside a solid
			element = 0.0f;
			centerElement++;
			}
		else{
			// Voxel is free (filled with "air")
			element = 1.0f;
			}
		}
	}

void NavierStokesSolver::buildMatrix(VolumeLevelSet* ls)
	{
	const float dx = mCellSize;
	const float dx2 = dx*dx;

	mMatrix.clear();

	const VolumeLevelSet::InsideMask& insideMask = ls->getInsideMask();
	for (unsigned int p = 0; p < insideMask.size(); p++){
		const Vector3<int>& pos = insideMask[p];
		int i = pos.x();
		int j = pos.y();
		int k = pos.z();

		FluidSolverSparseMatrix::Row row;
		row.xp1 = 0.0;
		row.xm1 = 0.0;
		row.yp1 = 0.0;
		row.ym1 = 0.0;
		row.zp1 = 0.0;
		row.zm1 = 0.0;
		row.c = -6.0;
		row.cPos = pos;

		//Make distinction of the voxels:
		// Fluid 1
		// Solid 0
		// Empty 1
		// Process voxels in x direction
		processVoxel(i+1,j,k, row.xp1, row.c,ls);
		processVoxel(i-1,j,k, row.xm1, row.c,ls);

		// Process voxels in y direction
		processVoxel(i,j+1,k, row.yp1, row.c,ls);
		processVoxel(i,j-1,k, row.ym1, row.c,ls);

		// Process voxels in z direction
		processVoxel(i,j,k+1, row.zp1, row.c,ls);
		processVoxel(i,j,k-1, row.zm1, row.c,ls);

		/*if (mMatrix.rowIsSingular(row)){
		std::cerr << "Warning! Poisson matrix contains singular rows!\n";
		printf("Row: %f %f %f %f %f %f %f\n", row.xp1,row.xm1,row.yp1,row.ym1,row.zp1,row.zm1,row.c);
		}*/

		row.xp1 /= dx2;
		row.xm1 /= dx2;
		row.yp1 /= dx2;
		row.ym1 /= dx2;
		row.zp1 /= dx2;
		row.zm1 /= dx2;
		row.c /= dx2;

		mMatrix.addRow(row);
		}
	}

void NavierStokesSolver::solvePoissonEquation(VolumeLevelSet* ls, float dt)
	{
	// Build pressure vector
	FluidSolverVector pressureVector;
	pressureVector.buildVector(mPressureField, ls);

	// Build Right Hand Side vector
	FluidSolverVector RHSVector;
	RHSVector.buildVector(mRHSField, ls);

	// Get the poisson matrix
	FluidSolverSparseMatrix& matrix = mMatrix;

	// Create additional data fields needed by the CG solver..
	Volume<float>* pField = new Volume<float>(mDimX, mDimY, mDimZ, 0.0f);
	Volume<float>* qField = new Volume<float>(mDimX, mDimY, mDimZ, 0.0f);
	Volume<float>* rField = new Volume<float>(mDimX, mDimY, mDimZ, 0.0f);

	// Solve the system using conjugate gradient.
	ConjugateGradient<FluidSolverSparseMatrix, FluidSolverVector, float> CGSolver(100, 1e-3, pField, qField, rField, ls);
	CGSolver.solve(matrix, pressureVector, RHSVector);

	printf("Projection poisson equation solved in %i iterations. Tolerance: %e\n", CGSolver.getNumIter(), CGSolver.getTolerance());

	delete pField;
	delete qField;
	delete rField;
	}


void NavierStokesSolver::velocityFieldCorrection(VolumeLevelSet* ls, float dt)
	{
	const float dx = mCellSize;

	const VolumeLevelSet::InsideMask& insideMask = ls->getInsideMask();
	for (unsigned int p = 0; p < insideMask.size(); p++){
		const Vector3<int>& pos = insideMask[p];
		int i = pos.x();
		int j = pos.y();
		int k = pos.z();

		//const float p0 = mPressureField->getValue(i,j,k);
		const float pXP1 = mPressureField->getValue(i+1,j,k);
		const float pXM1 = mPressureField->getValue(i-1,j,k);

		const float pYP1 = mPressureField->getValue(i,j+1,k);
		const float pYM1 = mPressureField->getValue(i,j-1,k);

		const float pZP1 = mPressureField->getValue(i,j,k+1);
		const float pZM1 = mPressureField->getValue(i,j,k-1);

		Vector3<float> newV = mOldField->getValue(i,j,k);

		newV.x() -= 0.5*(pXP1 - pXM1)/dx;
		newV.y() -= 0.5*(pYP1 - pYM1)/dx;
		newV.z() -= 0.5*(pZP1 - pZM1)/dx;

		mOldField->setValue(i,j,k, newV);
		}
	}

void NavierStokesSolver::calculateEnergy(VolumeLevelSet* ls, float& potentialEnergy, float& kineticEnergy)
{
	potentialEnergy = 0.0f;
	kineticEnergy	= 0.0f;

	const float oo_dx = 1.0/mCellSize;
	const float epsilon = 1.5;

	const VolumeLevelSet::InsideMask& insideMask = ls->getInsideMask();
	for (unsigned int p = 0; p < insideMask.size(); p++)
	{
		const Vector3<int>& pos = insideMask[p];
		int i = pos.x();
		int j = pos.y();
		int k = pos.z();

		// Compute the voxel mass using a Heavy-Side Function
		const float phi0 = ls->getValue(i,j,k)*oo_dx;

		float voxelMass;
		if (phi0 > epsilon){
			voxelMass = 0.0;
			}
		else if (phi0 < -epsilon){
			voxelMass = 1.0;
			}
		else{
			voxelMass = 0.5 - phi0/(epsilon*2.0) - 1/(2.0*M_PI)*sin(phi0*M_PI/epsilon);
			}

		// grid to world coordinates
		float x, height, z;
		ls->grid2World(i, j, k, x, height, z);

		// Velocity
		Vector3<float> velocity = ls->getVelocityField()->getValue(i, j, k);
		
		// Kinetic Energy
		kineticEnergy += voxelMass * (velocity*velocity);

		// Potential Energy
		potentialEnergy += voxelMass * mG * height;
	}

	// Scale the final energy to the volume
	kineticEnergy *= mCellSize*mCellSize*mCellSize;
	potentialEnergy *= mCellSize*mCellSize*mCellSize;
}

float NavierStokesSolver::calculateVolume(VolumeLevelSet* ls)
	{

	const float dx = mCellSize;
	const float oo_dx = 1.0/dx;
	const float epsilon = 1.5;
	float volume = 0;


	const VolumeLevelSet::InsideMask& insideMask = ls->getInsideMask();
	for (unsigned int p = 0; p < insideMask.size(); p++){
		const Vector3<int>& pos = insideMask[p];
		int i = pos.x();
		int j = pos.y();
		int k = pos.z();

		const float phi0 = ls->getValue(i,j,k)*oo_dx;

		float H;
		if (phi0 > epsilon){
			H = 0.0;
			}
		else if (phi0 < -epsilon){
			H = 1.0;
			}
		else{
			H = 0.5 - phi0/(epsilon*2.0) - 1/(2.0*M_PI)*sin(phi0*M_PI/epsilon);
			}
		volume += H;
		}
	volume *= dx*dx*dx;
	return volume;
	}

void NavierStokesSolver::calculateNewTimestep(std::vector<Geometry*>& geometryList)
	{
	//Build list of solids in scene
	std::vector<LevelSet*> solids;
	VolumeLevelSet* fluid = NULL;
	for (unsigned int i = 0; i < geometryList.size(); i++){
		if (typeid(*geometryList[i]) == typeid(VolumeLevelSet))
			{
			fluid = dynamic_cast<VolumeLevelSet *>(geometryList[i]);
			}
		else{
			if (typeid(*geometryList[i]) == typeid(LevelSet))
				{
				solids.push_back(dynamic_cast<LevelSet *>(geometryList[i]));
				}
			}
		}


	// Iterate trough narrow band and find highest speed (velocity magnitude)
	LevelSetGrid::Iterator iter = fluid->getLevelSetGrid().beginNarrowBand();
	LevelSetGrid::Iterator iend = fluid->getLevelSetGrid().endNarrowBand();

	const float numSolids = solids.size();
	const float dx = mCellSize;
	float speed = 0;
	float maxTimestep = std::numeric_limits<float>::max();
	while (iter != iend) 
		{
		int i = iter.getI();
		int j = iter.getJ();
		int k = iter.getK();

		const Vector3<float>& v = mOldField->getValue(i,j,k);
		const float vLen = v.length();
		speed = std::max(speed, vLen);

		// Compute distance vector to solid at this point
		// When a hit is predicted decrease the time step
		// to prevent intersection
		for (int solidId = 0; solidId < numSolids; solidId++)
			{
			const LevelSet& solid = *(solids[solidId]);

			// Calculate normal
			Vector3<float> normal( solid.diffXpm(i,j,k), solid.diffYpm(i,j,k), solid.diffZpm(i,j,k));
			normal.normalize();

			// Create distance vector
			const float closestDistToSolid = fabs(solid.getValue(i,j,k));
			Vector3<float> distance = normal*closestDistToSolid;

			// Calculate speed towards closest point on solid 
			// (project the v into the normal direction)
			const float speed2 = fabs(distance*v);

			// Calculate time to reach the closest point given the current velocity
			if (speed2 > 0.001 * dx)
				{
				const float time = closestDistToSolid/speed2;
				maxTimestep = std::min(maxTimestep, time);
				}
			}

		iter++;
		}
	// Calculate largest timestep that satisfies the CFL condition. Assumes velocity is in unit m/s
	const float alpha = 0.7;
	mLargestDT = std::min(alpha*mCellSize/speed, maxTimestep);
	}


