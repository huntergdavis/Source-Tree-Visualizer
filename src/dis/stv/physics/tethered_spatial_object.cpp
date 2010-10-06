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
double TetheredSpatialObject::updateLocation(double timeDelta)
{
	DiscursiveDebugPrint("spatial physics","Updating location to...\n");
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
			DiscursiveDebugPrint("spatial physics","(%f, %f)\n",ndx + this->tetherX,ndy + this->tetherY);
		}
		// Since tether won't allow the full movement
		// calculate distance we can move before it becoming taut.
		else
		{
//			double b = -2 * dist * cos(atan(ndy/ndx)-atan(dy/dx));
//			double c = (dist * dist) - (tetherRadius * tetherRadius);
//			double n_a = -(b + sqrt((b * b) - 4 * c))/2;
//			double n_b = -(b - sqrt((b * b) - 4 * c))/2;

			// Split segment into smaller chunks and find closest
			// chunk length to give a radius equal to tetherRadius
			double offs;
			int chunks = 1000;
			double chunkX = (ndx - dx)/chunks;
			double chunkY = (ndy - dy)/chunks;
			int upper = chunks;
			int lower = 0;
			int mid;
			int chunksUsed = 0;
			while(upper > lower)
			{
				mid = ((upper - lower)/2) + lower;
				//DiscursiveDebugPrint("[%d,%d,%d]\n",upper,mid,lower);
				ndx = dx + (mid*chunkX);
				ndy = dy + (mid*chunkY);
				// If the radius is still less than the tether, we can
				// go further out.
				if(sqrt( ndx * ndx + ndy * ndy ) < this->tetherRadius)
				{
					if(mid == upper - 1)
					{
						offs = atan((dy + (upper * chunkY))/(dx + (upper * chunkX))) - atan(dy/dx);
						chunksUsed = upper;
						break;
					}
					else
					{
						lower = mid + 1;
					}
				}
				// We must go less far
				else if(sqrt( ndx * ndx + ndy * ndy ) > this->tetherRadius)
				{
					if(mid == lower + 1)
					{
						offs = atan((dy + (lower * chunkY))/(dx + (lower * chunkX))) - atan(dy/dx);
						chunksUsed = lower;
						break;
					}
					else
					{
						upper = mid - 1;
					}
				}
				// Some how we hit it exactly
				else
				{
					offs = atan(ndy/ndx) - atan(dy/dx);
					chunksUsed = mid;
					break;
				}
			}

			DiscursiveDebugPrint("spatial physics","(%f,%f)~(%f,%f) -> %f\n",dx,dy,ndx,ndy,offs);
			// Use new rotation to get intermediate x,y
			double newRot = atan(dy/dx) + offs;
			dx = this->tetherRadius * cos(newRot);
			dy = this->tetherRadius * sin(newRot);
			// Do normal calculation with taut line for remainder or distance
			// Calculate mag of accel tangential to tether
			double aTangentX = 0;
			double aTangentY = this->getAccelY();
			if(dy != 0)
			{
				if(dx != 0)
				{
					double rotFromCenter = (3.14159/2) - atan(dy/dx);
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
			// Convert cartesian movement into polar movement.  Final term is for remainder of distance
			double dRot = (timeDelta * timeDelta) * (aTangentMag/this->tetherRadius) * ((1000-chunksUsed)/1000.0);
			// Calculate current polar rotation.
			// Note, this uses a different polar coord system than the first
			// portion of this function.
			double currRot = 3.14159/2;
			if(dx != 0)
			{
				currRot = tan(dy/dx);
			}
			// Calculate new polar rotation
			double finalRot = currRot + dRot;
			// Convert new polar rotation to cartesian coords
			double finalX = this->tetherRadius * cos(finalRot);
			double finalY = this->tetherRadius * sin(finalRot);
			// Update location
			this->setLocation(finalX, finalY);
			DiscursiveDebugPrint("spatial physics","(%f, %f)\n",finalX,finalY);
		}
	}
	// Tether is taut
	else
	{
		DiscursiveDebugPrint("spatial physics","tether is taut\n");
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
		DiscursiveDebugPrint("spatial physics","Acceleration: <%f,%f>  Tangent @ (%f,%f): <%f,%f>\n",this->getAccelX(),this->getAccelY(),this->getX(),this->getY(),aTangentX,aTangentY);
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
		DiscursiveDebugPrint("spatial physics","(%f, %f)\n",finalX,finalY);
	}
	return 0;
}
