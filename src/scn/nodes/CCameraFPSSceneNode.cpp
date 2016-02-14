//|-------------------------------------------------------------------------
//| File:        CCameraFPSSceneNode.cpp
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------

#include "CCameraFPSSceneNode.h"
#include <io/IInputDispatcher.h>  
#include <scn/ISceneManager.h>

//---------------------------------------------------------------------------
namespace my {
namespace scn {
//---------------------------------------------------------------------------

CCameraFPSSceneNode::CCameraFPSSceneNode( 
    ISceneNode* player, s32 id,              
    f32 rot_speed, f32 move_speed, f32 zoom_speed
	) : CCameraSceneNode(SCENE_MANAGER.getRootSceneNode(), id, new CCameraFPSController)
{
#if MY_DEBUG_MODE 
    IUnknown::setClassName("CCameraFPSSceneNode");
#endif
    setRotationSpeed(rot_speed);
    setZoomSpeed(zoom_speed);
    setMoveSpeed(move_speed);
	if (player)
		player->attachCamera(this);
}

//---------------------------------------------------------------------------

CCameraFPSSceneNode::~CCameraFPSSceneNode()
{
}

//---------------------------------------------------------------------------

void CCameraFPSController::animate(ICameraSceneNode *camera, u32 timeMs)
{
	ICameraController::animate(camera, timeMs);

	io::IInputDispatcher &inpd = io::IInputDispatcher::getSingleton();
	io::ICursorControl   &cursor = io::ICursorControl::getSingleton();

    if (m_FirstUpdate)
    {		
        cursor.setRelativePosition(0.5f, 0.5f);
    }

	f32 zoom_val	= -0.0005f * getTimeDiff() * camera->getZoomSpeed();
	f32 mov_speed	=  0.002f * getTimeDiff() * camera->getMoveSpeed();
	f32 rot_speed	= -1.0f * camera->getRotationSpeed();

	f32 rotationX = 0.0f, rotationY = 0.0f;

	core::position2d<f32> cursorpos = cursor.getRelativePosition();
      
    static f32 rotX[3]={0.0f, 0.0f, 0.0f}, rotY[3]={0.0f, 0.0f, 0.0f};

    if (m_FirstUpdate)
    { 
		rotX[0] = rotX[1] = 0.5f - cursorpos.X; 
        rotY[0] = rotY[1] = 0.5f - cursorpos.Y; 
    }
    rotX[2] = 0.5f - cursorpos.X; // puts values from this frame 
    rotY[2] = 0.5f - cursorpos.Y; // into table 

    if (cursorpos.X < 0.5 || cursorpos.X > 0.5 || 
        cursorpos.Y < 0.5 || cursorpos.Y > 0.5) 
    { 
		rotationX = - (rotX[0]+rotX[1]+rotX[2])/3 * rot_speed;
		rotationY = - (rotY[0]+rotY[1]+rotY[2])/3 * rot_speed;
		cursor.setRelativePosition( 0.5, 0.5 ); 
	}
	rotX[0] = rotX[1]; // finaly we move values to make room for a new ones 
	rotX[1] = rotX[2]; 
	rotY[0] = rotY[1]; 
	rotY[1] = rotY[2]; 	

	core::vector3df tgtVec = m_Tgt - m_Pos;
	tgtVec.normalize();
	tgtVec *= 100;

    // move
    core::vector3df movedir = tgtVec;
    movedir.normalize();	
    if (inpd.getActionControlState(game::EGA_MOVE_FORWARD))		
		m_Pos += movedir * mov_speed;
    if (inpd.getActionControlState(game::EGA_MOVE_BACKWARD))
        m_Pos -= movedir * mov_speed;

    // strafe
    core::vector3df strafevect = tgtVec;
    strafevect = strafevect.getCrossProduct(m_Up);
    strafevect.normalize();
    if (inpd.getActionControlState(game::EGA_MOVE_LEFT_STRAFE))
        m_Pos += strafevect * mov_speed;
    if (inpd.getActionControlState(game::EGA_MOVE_RIGHT_STRAFE))
        m_Pos -= strafevect * mov_speed;

	m_Tgt = m_Pos + tgtVec;
	m_Tgt.rotateXZByDegrees(rotationX, m_Pos);

	m_Tgt.Y += rotationY;
	tgtVec = m_Tgt - m_Pos;
	tgtVec.normalize();
	tgtVec *= 100;
	m_Tgt = m_Pos + tgtVec;

	camera->setPosition(m_Pos);
	camera->setTarget(m_Tgt);

	m_FirstUpdate = false;
}

//---------------------------------------------------------------------------
} // end namespace scn
} // end namespace my
//--------------------------------------------------------------------------- 