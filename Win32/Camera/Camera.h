#pragma once

class CCamera
{
protected:
	D2D_POINT_2F					m_d2dptPosition;
	D2D_MATRIX_3X2_F				m_d2dmtxWorld;
	D2D_MATRIX_3X2_F				m_d2dmtxView;

	D2D_SIZE_F						m_szClientSize;
	bool							m_bControlLock;
	float							m_fScale;

private:
	void SetMatrix();

public:
	CCamera();
	virtual ~CCamera();

	D2D1_POINT_2F GetCameraPosition(){ return m_d2dptPosition; }
	virtual void Move(D2D1_POINT_2F& d2dptShift) { m_d2dptPosition = m_d2dptPosition + d2dptShift; }
	virtual void SetPosition(D2D1_POINT_2F& ptPosition) { m_d2dptPosition = ptPosition; }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	virtual void Scale(float Scale){ m_fScale = Scale; }
	float GetScale(){ return m_fScale; }
	void AgjustScale(float Scale){ m_fScale += Scale; }
	virtual void SetLookAt(D2D1_POINT_2F& d2dptLookAt) { }
	D2D_MATRIX_3X2_F GetCameraMatrix(){ SetMatrix(); return m_d2dmtxWorld; }
	D2D_MATRIX_3X2_F RegenerateViewMatrix();

	void Init(D2D_SIZE_F szClientSize);
	bool CheckControlLock(){ return m_bControlLock; }

};
