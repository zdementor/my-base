//|-------------------------------------------------------------------------
//| File:        ODEColliders.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2016 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include <scn/ITerrainSceneNode.h>
#include <scn/ISceneManager.h>
#include <vid/IVideoDriver.h>

#include <heightfield.h>

//---------------------------------------------------------------------------
namespace my {
namespace dyn {
namespace ode {
//---------------------------------------------------------------------------

#define NUMC_MASK (0xffff)

int CollideHeightfieldRay(
	dxGeom *o1, dxGeom *rayGeom, int flags, dContactGeom *contact, int skip)
{
	assert( skip >= (int)sizeof(dContactGeom) );
	assert( rayGeom->type == dRayClass );
	assert( o1->type == dHeightfieldClass );
	assert((flags & NUMC_MASK) >= 1);

	const int maxContacts = (flags & NUMC_MASK);
	int numContacts = 0;

	dxHeightfield *hf = (dxHeightfield*)o1;
	dxHeightfieldData *hfdata = hf->m_p_data;
	scn::ITerrainSceneNode *terrain = (scn::ITerrainSceneNode *)hfdata->m_pUserData;

	const f32 hScale = terrain->getHeightScale();
	const f32 cellSpace = terrain->getGridPointSpacing();
	const f32 cellSpaceDiv2 = cellSpace / 2.f;
	const s32 hfsize = terrain->getHeightFieldSize();
	const s32 hfsize_1 = hfsize - 1;
	const s32 hfsize_2 = hfsize - 2;

	const core::matrix4 hftransf = terrain->getAbsoluteTransformation();
	core::matrix4 hftransf_inv = hftransf;
	hftransf_inv.makeInversed();

	dReal rLen = 0.f;
	dVector3 rOrig, rDir;
	dGeomRayGet(rayGeom, rOrig, rDir);
	rLen = dGeomRayGetLength(rayGeom);
	core::line3df ray(rOrig[0], rOrig[1], rOrig[2],
		rOrig[0] + rDir[0] * rLen,
			rOrig[1] + rDir[1] * rLen,
				rOrig[2] + rDir[2] * rLen);

	hftransf_inv.transformVect(ray.start);
	hftransf_inv.transformVect(ray.end);
	f32 rDXPow2 = (ray.end.X - ray.start.X); rDXPow2 *= rDXPow2;
	f32 rDZPow2 = (ray.end.Z - ray.start.Z); rDZPow2 *= rDZPow2;
	f64 rDHPow2 = (ray.end.Y - ray.start.Y); rDHPow2 *= rDHPow2;

	const core::vector3df p0 = terrain->getCellPosition(0, hfsize_1);
	const core::vector3df pn = terrain->getCellPosition(hfsize_1, 0);

	const core::rectf rct(p0.X, p0.Z, pn.X, pn.Z);
	const f32 rctWidth = rct.getWidth();
	const f32 rctHeight = rct.getHeight();

	const core::line2df rayxz(ray.start.X, ray.start.Z, ray.end.X, ray.end.Z);
	const core::line2df line01(rct.Left,  rct.Top,    rct.Right, rct.Top);
	const core::line2df line12(rct.Right, rct.Top,    rct.Right, rct.Bottom);
	const core::line2df line23(rct.Right, rct.Bottom, rct.Left,  rct.Bottom);
	const core::line2df line30(rct.Left,  rct.Bottom, rct.Left,  rct.Top);

	u32 rIntersectsCnt = 0;
	core::vector2df rIntersects[2];

	if (rayxz.start.X >= rct.Left && rayxz.start.X <= rct.Right
			&& rayxz.start.Y >= rct.Top && rayxz.start.Y <= rct.Bottom)
		rIntersects[rIntersectsCnt++] = rayxz.start;
	if (rayxz.end.X >= rct.Left && rayxz.end.X <= rct.Right
			&& rayxz.end.Y >= rct.Top && rayxz.end.Y <= rct.Bottom)
		rIntersects[rIntersectsCnt++] = rayxz.end;
	if (rIntersectsCnt < 2 && line01.intersectWith(rayxz, rIntersects[rIntersectsCnt]))
		rIntersectsCnt++;
	if (rIntersectsCnt < 2 && line12.intersectWith(rayxz, rIntersects[rIntersectsCnt]))
		rIntersectsCnt++;
	if (rIntersectsCnt < 2 && line23.intersectWith(rayxz, rIntersects[rIntersectsCnt]))
		rIntersectsCnt++;
	if (rIntersectsCnt < 2 && line30.intersectWith(rayxz, rIntersects[rIntersectsCnt]))
		rIntersectsCnt++;

	if (rIntersectsCnt == 2
			&& !core::math::Equals(rIntersects[0].X, rIntersects[1].X)
			&& !core::math::Equals(rIntersects[0].Y, rIntersects[1].Y))
	{
		const core::vector2di rayCells[2] =
		{
			core::vector2di(
				(f32)hfsize_1 * (rayxz.start.X - rct.Left) / rctWidth + 0.5f,
				(f32)hfsize_1 * (rayxz.start.Y - rct.Top) / rctHeight + 0.5f),
			core::vector2di(
				(f32)hfsize_1 * (rayxz.end.X - rct.Left) / rctWidth + 0.5f,
				(f32)hfsize_1 * (rayxz.end.Y - rct.Top) / rctHeight + 0.5f),
		};
		const core::vector2di rIntersectCells[2] =
		{
			core::vector2di(
				(f32)hfsize_1 * (rIntersects[0].X - rct.Left) / rctWidth + 0.5f,
				(f32)hfsize_1 * (rIntersects[0].Y - rct.Top) / rctHeight + 0.5f),
			core::vector2di(
				(f32)hfsize_1 * (rIntersects[1].X - rct.Left) / rctWidth + 0.5f,
				(f32)hfsize_1 * (rIntersects[1].Y - rct.Top) / rctHeight + 0.5f),
		};

		const f64 dX0 = cellSpace * (rIntersectCells[0].X - rayCells[0].X);
		const f64 dZ0 = cellSpace * (rIntersectCells[0].Y - rayCells[0].Y);
		const f64 dX1 = cellSpace * (rIntersectCells[1].X - rayCells[0].X);
		const f64 dZ1 = cellSpace * (rIntersectCells[1].Y - rayCells[0].Y);
		const f64 dLen0Pow2 = dX0 * dX0 + dZ0 * dZ0;
		const f64 dLen1Pow2 = dX1 * dX1 + dZ1 * dZ1;
		const f64 rDLenPow2 = rDXPow2 + rDZPow2;

		f64 riH0 = ray.start.Y;
		if (!!dLen0Pow2)
			riH0 = riH0 + core::math::Sqrt(rDHPow2 * (dLen0Pow2 / rDLenPow2));
		f64 riH1 = riH0 + core::math::Sqrt(rDHPow2 * (dLen1Pow2 / rDLenPow2));

		static core::array <core::vector2di> trace_way;
		core::math::draw_line(
			rIntersectCells[0].X, rIntersectCells[0].Y,
			rIntersectCells[1].X, rIntersectCells[1].Y,
			trace_way);
		const s32 tCnt = trace_way.size();
		const s32 tCnt_1 = tCnt - 1;
		const f32 riDHPerCell = (tCnt>0) ? ((riH1-riH0)/tCnt) : 0.;
#if 0
		for (s32 t = 0; t < tCnt_1;	t++)
		{
			s32 iCur = trace_way[t].X;
			s32 jCur = hfsize_1 - trace_way[t].Y;

			core::vector3df cellPosCur = terrain->getCellPosition(iCur, jCur);

			core::aabbox3df bb(
				-3+cellPosCur.X, -3+cellPosCur.Y, -3+cellPosCur.Z,
				+3+cellPosCur.X, +3+cellPosCur.Y, +3+cellPosCur.Z);
			VIDEO_DRIVER.register3DBoxForRendering(vid::ERP_3D_TRANSP_1ST_PASS, hftransf,
				bb, 0xff0000ff);
		}
		VIDEO_DRIVER.register3DLineForRendering(vid::ERP_3D_TRANSP_1ST_PASS, hftransf,
			ray.start, ray.end, 0xffffffff);
#endif
		f32 riH = riH0;
		for (s32 t = 0; t < tCnt_1 && numContacts < maxContacts;
				t++, riH += riDHPerCell)
		{
			s32 iCur = trace_way[t].X;
			s32 jCur = hfsize_1 - trace_way[t].Y;

			s32 iNext = trace_way[t + 1].X;
			s32 jNext = hfsize_1 - trace_way[t + 1].Y;

			f32 riHNext = riH + riDHPerCell;

			core::vector3df cellPosCur = terrain->getCellPosition(iCur, jCur);
			core::vector3df cellPosNext = terrain->getCellPosition(iNext, jNext);

			bool cellHigherCur = ((cellPosCur.Y) > riH);
			bool cellHigherNext = ((cellPosNext.Y) > riHNext);

			if (cellHigherCur != cellHigherNext)
			{
				bool found = false;
				core::vector3df intersect(0, 0, 0);

				if (iNext > 2 && iNext < hfsize_2 && jNext > 2 && jNext < hfsize_2)
				{
					core::vector3df cpNextTT  = terrain->getCellPosition(iNext,   jNext+2);
					core::vector3df cpNextLT  = terrain->getCellPosition(iNext-1, jNext+1);
					core::vector3df cpNextT   = terrain->getCellPosition(iNext,   jNext+1);
					core::vector3df cpNextRT  = terrain->getCellPosition(iNext+1, jNext+1);
					core::vector3df cpNextLL  = terrain->getCellPosition(iNext-2, jNext  );
					core::vector3df cpNextL   = terrain->getCellPosition(iNext-1, jNext  );
					core::vector3df &cpNextO  = cellPosNext;
					core::vector3df cpNextR   = terrain->getCellPosition(iNext+1, jNext  );
					core::vector3df cpNextRR  = terrain->getCellPosition(iNext+2, jNext  );
					core::vector3df cpNextLB  = terrain->getCellPosition(iNext-1, jNext-1);
					core::vector3df cpNextB   = terrain->getCellPosition(iNext,   jNext-1);
					core::vector3df cpNextRB  = terrain->getCellPosition(iNext+1, jNext-1);
					core::vector3df cpNextBB  = terrain->getCellPosition(iNext,   jNext-2);

					core::triangle3df triCollid[] =
					{
						core::triangle3df(cpNextL, cpNextLL, cpNextLB),
						core::triangle3df(cpNextL, cpNextLB, cpNextO ),
						core::triangle3df(cpNextL, cpNextO,  cpNextLT),
						core::triangle3df(cpNextL, cpNextLT, cpNextLL),

						core::triangle3df(cpNextB, cpNextLB, cpNextBB),
						core::triangle3df(cpNextB, cpNextBB, cpNextRB),
						core::triangle3df(cpNextB, cpNextRB, cpNextO ),
						core::triangle3df(cpNextB, cpNextO,  cpNextLB),

						core::triangle3df(cpNextR, cpNextO,  cpNextRB),
						core::triangle3df(cpNextR, cpNextRB, cpNextRR),
						core::triangle3df(cpNextR, cpNextRR, cpNextRT),
						core::triangle3df(cpNextR, cpNextRT, cpNextO ),

						core::triangle3df(cpNextT, cpNextLT, cpNextO ),
						core::triangle3df(cpNextT, cpNextO,  cpNextRT),
						core::triangle3df(cpNextT, cpNextRT, cpNextTT),
						core::triangle3df(cpNextT, cpNextTT, cpNextLT),
					};

					u32 intersectsCnt = 0;
					core::vector3df intersects[sizeof(triCollid) / sizeof(*triCollid)];

					for (u32 ti = 0; intersectsCnt < sizeof(triCollid) / sizeof(*triCollid)
							&& ti < sizeof(triCollid) / sizeof(*triCollid); ti++)
					{
						if (triCollid[ti].getIntersectionWithLimitedLine(ray, intersects[intersectsCnt]))
							intersectsCnt++;
					}

					if (intersectsCnt > 0)
					{
						found = true;

						if (intersectsCnt == 1)
							intersect.set(intersects[0]);
						else
						{
							f32 lenSQMin = (intersects[0] - ray.start).getLengthSQ();
							u32 idxMin = 0;

							for (u32 i = 1; i < intersectsCnt; i++)
							{
								f32 lenSQ = (intersects[i] - ray.start).getLengthSQ();
								if (lenSQ < lenSQMin)
								{
									lenSQMin = lenSQ;
									idxMin = i;
								}
							}
							intersect.set(intersects[idxMin]);
						}
					}
#if 0
					core::aabbox3df bb(
						-3+cellPosCur.X, -3+cellPosCur.Y, -3+cellPosCur.Z,
						+3+cellPosCur.X, +3+cellPosCur.Y, +3+cellPosCur.Z);
					core::aabbox3df bb2(
						-3+cellPosNext.X, -3+cellPosNext.Y, -3+cellPosNext.Z,
						+3+cellPosNext.X, +3+cellPosNext.Y, +3+cellPosNext.Z);
					VIDEO_DRIVER.register3DBoxForRendering(vid::ERP_3D_TRANSP_1ST_PASS, hftransf,
							bb, 0xffff0000);
					VIDEO_DRIVER.register3DBoxForRendering(vid::ERP_3D_TRANSP_1ST_PASS, hftransf,
							bb2, 0xffff0000);
					for (u32 ti = 0; ti < sizeof(triCollid) / sizeof(*triCollid); ti++)
					{
						VIDEO_DRIVER.register3DLineForRendering(vid::ERP_3D_TRANSP_1ST_PASS, hftransf,
							triCollid[ti].A, triCollid[ti].B, 0xffffff00);
						VIDEO_DRIVER.register3DLineForRendering(vid::ERP_3D_TRANSP_1ST_PASS, hftransf,
							triCollid[ti].B, triCollid[ti].C, 0xffffff00);
						VIDEO_DRIVER.register3DLineForRendering(vid::ERP_3D_TRANSP_1ST_PASS, hftransf,
							triCollid[ti].C, triCollid[ti].A, 0xffffff00);
					}
#endif
				}

				if (found)
				{
					core::vector3df orig(0.f, 0.f, 0.f);

					core::vector3df normal = terrain->getNormal(intersect.X, intersect.Z);
					normal *= -1.f; // must invert normal here, ode will onvert back it later

					// back to the outer space
					hftransf.transformVect(intersect);
					hftransf.transformVect(normal);
					hftransf.transformVect(orig);
					normal = normal - orig;

					dContactGeom &c = contact[numContacts++];
					c.pos[0] = intersect.X;
					c.pos[1] = intersect.Y;
					c.pos[2] = intersect.Z;
					c.depth = 0.f;
					c.normal[0] = normal.X;
					c.normal[1] = normal.Y;
					c.normal[2] = normal.Z;
					c.g1 = o1;
					c.g2 = rayGeom;
				}
			}
		}
	}

	return numContacts;
}
//---------------------------------------------------------------------------
} // end namespace ode
} // end namespace dyn
} // end namespace my
//---------------------------------------------------------------------------
