#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera()
{
	m_d2dmtxWorld = Matrix3x2F::Identity();
	m_d2dptPosition = Point2F(0.f, 0.f);
	m_bControlLock = false;
	m_fScale = 1.f;
}

CCamera::~CCamera()
{
}

void CCamera::Init(D2D_SIZE_F szClientSize)
{
	m_szClientSize = szClientSize;
}

void CCamera::SetMatrix()
{
	m_d2dmtxWorld = Matrix3x2F::Identity();
	m_d2dmtxWorld = Matrix3x2F::Scale(m_fScale, m_fScale) * m_d2dmtxWorld;
	m_d2dmtxWorld._31 = m_d2dptPosition.x;
	m_d2dmtxWorld._32 = m_d2dptPosition.y;
}

D2D_MATRIX_3X2_F CCamera::RegenerateViewMatrix()
{
	SetMatrix();
	D2D_POINT_2F m_vRight = { m_d2dmtxWorld._11, m_d2dmtxWorld._12 };
	D2D_POINT_2F m_vUp = { m_d2dmtxWorld._21, m_d2dmtxWorld._22 };
	m_d2dmtxView._11 = m_vRight.x;
	m_d2dmtxView._21 = m_vRight.y;
	m_d2dmtxView._12 = m_vUp.x;
	m_d2dmtxView._22 = m_vUp.y;
	m_d2dmtxView._31 = -(m_d2dptPosition * m_vRight);
	m_d2dmtxView._32 = -(m_d2dptPosition * m_vUp);
	return m_d2dmtxView;
}
