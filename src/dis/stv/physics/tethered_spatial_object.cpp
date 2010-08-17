/*
 * tethered_spatial_object.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#include "tethered_spatial_object.h"

TetheredSpatialObject::TetheredSpatialObject(double x, double y, double tetherX, double tetherY, double tetherRadius) : SpatialObject(x,y), tetherX( tetherX ), tetherY( tetherY ), tetherRadius( tetherRadius )
{
}

double TetheredSpatialObject::getTetherX()
{
	return this->tetherX;
}

double TetheredSpatialObject::getTetherY()
{
	return this->tetherY;
}

double TetheredSpatialObject::getTetherRadius()
{
	return this->tetherRadius;
}

// Update location based on acceleration and update constraints (tether)
void TetheredSpatialObject::updateLocation(double timeDelta)
{
	// Test unmodified acceleration values against
	// tether location constraints:
	// First check that distance to tether is less than
	// tether radius
	double dx = this->getX() - this->tetherX;
	double dy = this->getY() - this->tetherY;
	double dist = sqrt( dx * dx + dy * dy );
	if(dist < tetherRadius)
	{

		double ndx = this->getX() + (this->getAccelX() * (timeDelta * timeDelta)) - this->tetherX;
		double ndy = this->getY() + (this->getAccelY() * (timeDelta * timeDelta)) - this->tetherY;
		double ndist = sqrt( ndx * ndx + ndy * ndy );
		// if full movement doesnt cause tether to become taut
		if(ndist < tetherRadius)
		{
			// Update location with new values
			this->setLocation(ndx + this->tetherX,ndy + this->tetherY);
		}
		// Since tether won't allow the full movement
		// calculate distance we can move before it becoming taut.
		else
		{
		}

	}
	// Tether is taut
	else
	{
		// Calculate mag of accel tangential to tether
		double aTangentX = 0;
		double aTangentY = this->getAccelY();
		if(this->getY() != 0)
		{
			if(this->getX() != 0)
			{
				double rotFromCenter = (3.14159/2) - atan(this->getY()/this->getX());
				if(rotFromCenter != (3.14159/2))
				{
					aTangentX = -1*cos(rotFromCenter) * this->getAccelX();
					aTangentY = sin(rotFromCenter) * this->getAccelY();
				}
			}
			else
			{
				aTangentX = this->getAccelX();
				aTangentY = 0;
			}
		}
		double aTangentMag = sqrt(aTangentX * aTangentX + aTangentY * aTangentY);
		// Convert cartesian movement into polar movement
		double dRot = (timeDelta * timeDelta) * (aTangentMag/this->tetherRadius);
		// Calculate current polar rotation.
		// Note, this uses a different polar coord system than the first
		// portion of this function.
		double currRot = 3.14159/2;
		if(this->getX() != 0)
		{
			currRot = tan(this->getY()/this->getX());
		}
		// Calculate new polar rotation
		double finalRot = currRot + dRot;
		// Convert new polar rotation to cartesian coords
		double finalX = this->tetherRadius * cos(finalRot);
		double finalY = this->tetherRadius * sin(finalRot);
		// Update location
		this->setLocation(finalX, finalY);
	}
}
