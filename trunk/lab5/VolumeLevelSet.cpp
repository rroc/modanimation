#include "VolumeLevelSet.h"
#include "ColorMap.h"

const float VolumeLevelSet::mInsideConstant = 0.5; //0.5

VolumeLevelSet::VolumeLevelSet(float dx) : LevelSet(dx)
{
  commonConstructorOps();
}

VolumeLevelSet::VolumeLevelSet(float dx, const Implicit & impl) : LevelSet(dx, impl)
{
  commonConstructorOps();
}

VolumeLevelSet::~VolumeLevelSet()
{
	delete mVelocityField;
	delete mAdvectionField;
	delete mOldAdvectionField;
	delete mInsideBoolMask;
}

void VolumeLevelSet::setNarrowBandWidth(float width)
{
	LevelSet::setNarrowBandWidth(width);
	buildMasks();
	clearDisplayList();
}

void VolumeLevelSet::commonConstructorOps()
{
  Vector3<unsigned int> gridDim = mGrid.getDimensions();
  mDisplayListExists = false;

  mVelocityField = new Volume<Vector3<float> >(gridDim.x(), gridDim.y(), gridDim.z(), Vector3<float>(0.0));
  mAdvectionField = new Volume<Vector3<float> >(gridDim.x(), gridDim.y(), gridDim.z(), Vector3<float>(0.0));
  mOldAdvectionField = new Volume<Vector3<float> >(gridDim.x(), gridDim.y(), gridDim.z(), Vector3<float>(0.0));
  mInsideBoolMask = new Volume<bool>(gridDim.x(), gridDim.y(), gridDim.z(), false);

  buildMasks();
}

void VolumeLevelSet::setBoundingBox(const Bbox & b)
{
	LevelSet::setBoundingBox(b);

	// Resizing vector field
  // Create a new grid with requested size
  Vector3<unsigned int> gridDim = mGrid.getDimensions();

  delete mVelocityField;
  mVelocityField = new Volume<Vector3<float> >(gridDim.x(), gridDim.y(), gridDim.z(), Vector3<float>(0.0));

  delete mAdvectionField;
  mAdvectionField = new Volume<Vector3<float> >(gridDim.x(), gridDim.y(), gridDim.z(), Vector3<float>(0.0));

  delete mOldAdvectionField;
  mOldAdvectionField = new Volume<Vector3<float> >(gridDim.x(), gridDim.y(), gridDim.z(), Vector3<float>(0.0));

  delete mInsideBoolMask;
  mInsideBoolMask = new Volume<bool>(gridDim.x(), gridDim.y(), gridDim.z(), false);

}

Volume<Vector3<float> >* VolumeLevelSet::getVelocityField()
{
  clearDisplayList();
  return mVelocityField;
}

void VolumeLevelSet::buildMasks()
{
	mVolumeMask.clear();
	mInsideMask.clear();

	Vector3<unsigned int> gridDim = mGrid.getDimensions();
	for (unsigned int i = 0; i < gridDim.x(); i++){
    for (unsigned int j = 0; j < gridDim.y(); j++){
      for (unsigned int k = 0; k < gridDim.z(); k++){
				if (mGrid.getValue(i,j,k) < mGrid.getOutsideConstant())
				{
          mVolumeMask.push_back(Vector3<unsigned int>(i,j,k));
				}

				if (mGrid.getValue(i,j,k) <= mInsideConstant*mDx)
				{
          mInsideBoolMask->setValue(i,j,k, true);
          mInsideMask.push_back(Vector3<unsigned int>(i,j,k));
				}
			}
		}
	}
}

const VolumeLevelSet::VolumeMask& VolumeLevelSet::getVolumeMask() const
{
	return mVolumeMask;
}

const VolumeLevelSet::InsideMask& VolumeLevelSet::getInsideMask() const
{
	return mInsideMask;
}

LevelSetGrid& VolumeLevelSet::getLevelSetGrid()
{
	return mGrid;
}

void VolumeLevelSet::draw()
{
	LevelSet::draw();

	if (mVelocityField != NULL){
		Bbox b = getBoundingBox();
		Vector3<float> extent = b.pMax - b.pMin;
		//float maxExtent = std::max(extent.x(), std::max(extent.y(), extent.z()));
		//unsigned int maxCells = imax(extent.x(), imax(extent.y(), extent.z()));
		//float scaling = mDx * maxExtent;

		glPushMatrix();


		glTranslatef(b.pMin.x(), b.pMin.y(), b.pMin.z());
		glScalef(mDx,mDx,mDx);
		//glTranslatef(-0.5,-0.5, -0.5);


    //std::cerr << mTransform << std::endl;

    /*GLfloat glMatrix[16];
    mTransform.toGLMatrix(glMatrix);

    // Load transform into modelview matrix
    glMultMatrixf(glMatrix);*/


    drawVolumeGrid();
    glPopMatrix();
	}
}

void VolumeLevelSet::drawVolumeGrid()
{
	if (!mDisplayListExists)
	{
    Vector3<unsigned int> gridDim = mGrid.getDimensions();
    Vector3<unsigned int> velGridDim(mVelocityField->getDimX(),mVelocityField->getDimY(),mVelocityField->getDimZ());
    assert(gridDim == velGridDim);

    mDisplayList = glGenLists(1);
    glNewList(mDisplayList,GL_COMPILE_AND_EXECUTE);
		ColorMap map;
		Vector3<float> color;

		glPointSize(5);
		glDisable(GL_LIGHTING);

    float minLen = std::numeric_limits<float>::max();
    float maxLen = std::numeric_limits<float>::min();

		for (unsigned int p = 0; p < mVolumeMask.size(); p++){
      Vector3<int>& pos = mVolumeMask.at(p);
      int i = pos.x();
      int j = pos.y();
      int k = pos.z();

			float x,y,z;
			x = (float)i;
			y = (float)j;
			z = (float)k;


      const Vector3<float> & v = mVelocityField->getValue(i,j,k);
      //const Vector3<float> & v = mAdvectionField->getValue(i,j,k);

      float length = v.length();

      if (length != 0.0){
        minLen = std::min(minLen, length);
        maxLen = std::max(maxLen, length);

        Vector3<float> n = v;
				n.normalize();
				color = map.map(n, -1, 1);
				glColor3f(color.x(), color.y(), color.z());
				glBegin(GL_LINES);
					glVertex3f(x,y,z);
					glVertex3f(x+v.x(),y+v.y(),z+v.z());
				glEnd();

				if (getValue(i,j,k) <= getInsideConstant()){
          glColor3f(0.0,mInsideBoolMask->getValue(i,j,k),1.0);
				}
				else{
          glColor3f(1.0,0,0.0);
				}

	    	/*glBegin(GL_POINTS);
			  	glVertex3f(x,y,z);
			  glEnd();*/
			}
			else{
  			/*glColor3f(1.0,1.0,0);
	    		glBegin(GL_POINTS);
			  	glVertex3f(x,y,z);
			  glEnd();*/
			}

		}
		glEnable(GL_LIGHTING);

    glEndList();
    mDisplayListExists = true;

    //printf("min: %e,  max: %e\n", minLen, maxLen);
	}
	else{
    glCallList(mDisplayList);
	}
}

void VolumeLevelSet::clearDisplayList()
{
  glDeleteLists(mDisplayList,1);
  mDisplayListExists = false;
}

bool VolumeLevelSet::isInside(int i, int j, int k) const
{
	return mInsideBoolMask->getValue(i,j,k);
}


void VolumeLevelSet::buildAdvectionField()
{
	// Velocity extension..
	const float dx = mDx;
	const float dt = 0.7;
	const float outsideConstant = mGrid.getOutsideConstant();

	const unsigned int numIters = (unsigned int)(fabsf(outsideConstant)/dt/dx) +1;
	//printf("numiters: %i\n", numIters, mGrid.getOutsideConstant(), (fabsf(mGrid.getOutsideConstant())/dt/dx));

	// Copy velocity field to advection field (within the narrow band)
	{
    LevelSetGrid::Iterator iter = getLevelSetGrid().beginNarrowBand();
    LevelSetGrid::Iterator iend = getLevelSetGrid().endNarrowBand();

		while (iter != iend){
      int i = iter.getI();
      int j = iter.getJ();
      int k = iter.getK();

      mAdvectionField->setValue(i,j,k, mVelocityField->getValue(i,j,k));

      iter++;
		}
	}



	for (unsigned int iteration = 0; iteration < numIters; iteration++){
		// Swap fields..
  	Volume<Vector3<float> >* temp = mOldAdvectionField;
  	mOldAdvectionField = mAdvectionField;
  	mAdvectionField = temp;

		// Loop trough narrow band..
    LevelSetGrid::Iterator iter = getLevelSetGrid().beginNarrowBand();
    LevelSetGrid::Iterator iend = getLevelSetGrid().endNarrowBand();

		while (iter != iend){
      int i = iter.getI();
      int j = iter.getJ();
      int k = iter.getK();

			const float phi0 = getValue(i,j,k);

			if (fabs(phi0) >= fabs(getInsideConstant()))
			{
	      Vector3<float> n(  getValue(i+1,j,k) - getValue(i-1,j,k),
	                         getValue(i,j+1,k) - getValue(i,j-1,k),
	                         getValue(i,j,k+1) - getValue(i,j,k-1));

				const float length = n.length();
				if (length != 0.0){
					n *= 1.0/length;
				}

	      const float signValue = phi0/sqrt(phi0*phi0 + dx*dx);

				Vector3<float> v = mOldAdvectionField->getValue(i,j,k);

				Vector3<float> vXP1 = mOldAdvectionField->getValue(i+1,j,k);
				Vector3<float> vXM1 = mOldAdvectionField->getValue(i-1,j,k);

				Vector3<float> vYP1 = mOldAdvectionField->getValue(i,j+1,k);
				Vector3<float> vYM1 = mOldAdvectionField->getValue(i,j-1,k);

				Vector3<float> vZP1 = mOldAdvectionField->getValue(i,j,k+1);
				Vector3<float> vZM1 = mOldAdvectionField->getValue(i,j,k-1);

        Vector3<float> v0 =  v - dt*(std::max(0.0f, signValue*n.x())*(v - vXM1) + std::min(0.0f, signValue*n.x())*(vXP1 - v) +
	                    std::max(0.0f, signValue*n.y())*(v - vYM1) + std::min(0.0f, signValue*n.y())*(vYP1 - v) +
	                    std::max(0.0f, signValue*n.z())*(v - vZM1) + std::min(0.0f, signValue*n.z())*(vZP1 - v));

        mAdvectionField->setValue(i,j,k, v0);

        // Write outside value to solver..
        if (phi0 > fabs(getInsideConstant()))
        {
          mVelocityField->setValue(i,j,k, v0);
        }
			}
			else{
        Vector3<float> v = mOldAdvectionField->getValue(i,j,k);
        mAdvectionField->setValue(i,j,k, v);
			}

			iter++;
		}
	}
}

NavierStokesVectorField* VolumeLevelSet::getAdvectionField()
{
	Matrix4x4<float> worldToObj = getTransform().inverse();
  NavierStokesVectorField* vf = new NavierStokesVectorField(*mAdvectionField, worldToObj, mDx, getBoundingBox());
  //NavierStokesVectorField* vf = new NavierStokesVectorField(*mVelocityField, worldToObj, mDx, getBoundingBox());


  return vf;
}





