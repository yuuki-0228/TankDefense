#include "CollisionManager.h"
#include "..\Sphere\Sphere.h"
#include "..\Capsule\Capsule.h"
#include "..\Box\Box.h"
#include "..\Ray\Ray.h"
#include "..\Mesh\Mesh.h"

namespace coll
{

	//---------------------------------------.
	// 球体同士の当たり判定.
	//---------------------------------------.
	bool IsSphereToSphere( CSphere* pMySphere, CSphere* pOppSphere )
	{
		// スフィアが nullptr なら終了.
		if( pMySphere	== nullptr ) return false;
		if( pOppSphere	== nullptr ) return false;

		// 半径が 0 以下なら終了.
		if( pMySphere->GetRadius()	<= 0.0f ) return false;
		if( pOppSphere->GetRadius()	<= 0.0f ) return false;

		// 二点間の距離を取得.
		const float length = D3DXVec3Length( &(pMySphere->GetPosition() - pOppSphere->GetPosition()) );
		// お互いの半径の合計を取得.
		const float totalRadius = pMySphere->GetRadius() + pOppSphere->GetRadius();

		// 2点間の距離が2つの半径の合計より大きいので.
		if( length > totalRadius ){
			// 衝突していない.
			pMySphere->SetHitOff();
			pOppSphere->SetHitOff();
			return false;
		}

		// 衝突している.
		pMySphere->SetHitOn();
		pOppSphere->SetHitOn();

		return true;
	}

	//---------------------------------------.
	// カプセル同士の当たり判定.
	//---------------------------------------.
	bool IsCapsuleToCapsule( CCapsule* pMyCapsule, CCapsule* pOppCapsule )
	{
		if( pMyCapsule	== nullptr ) return false;
		if( pOppCapsule	== nullptr ) return false;

		// 半径が 0 以下なら終了.
		if( pMyCapsule->GetRadius()	<= 0.0f ) return false;
		if( pOppCapsule->GetRadius()<= 0.0f ) return false;

		SSegment mySeg	= pMyCapsule->GetSegment();
		SSegment oppSeg	= pOppCapsule->GetSegment();

		float d = CalcSegmentSegmentDist( mySeg, oppSeg );
		if( d >= pMyCapsule->GetRadius()+pOppCapsule->GetRadius() ){
			// 衝突していない.
			pMyCapsule->SetHitOff();
			pOppCapsule->SetHitOff();
			return false;
		}

		// 衝突している.
		pMyCapsule->SetHitOn();
		pOppCapsule->SetHitOn();

		return true;
	}

	//---------------------------------------.
	// ボックス同士の当たり判定.
	//---------------------------------------.
	bool IsOBBToOBB( CBox* pMyBox, CBox* pOppBox )
	{
		if( pMyBox	== nullptr ) return false;
		if( pOppBox	== nullptr ) return false;

		auto LenSegOnSeparateAxis = []( D3DXVECTOR3 *Sep, D3DXVECTOR3 *e1, D3DXVECTOR3 *e2, D3DXVECTOR3 *e3 = 0 )
		{
			// 3つの内積の絶対値の和で投影線分長を計算.
			// 分離軸Sepは標準化されていること.
			FLOAT r1 = fabs(D3DXVec3Dot( Sep, e1 ));
			FLOAT r2 = fabs(D3DXVec3Dot( Sep, e2 ));
			FLOAT r3 = e3 ? (fabs(D3DXVec3Dot( Sep, e3 ))) : 0;
			return r1 + r2 + r3;
		};

		// 各方向ベクトルの確保.
		// （N***:標準化方向ベクトル）.
		D3DXVECTOR3 NAe1 = pMyBox->GetDirection(0), Ae1 = NAe1 * pMyBox->GetScale().x;
		D3DXVECTOR3 NAe2 = pMyBox->GetDirection(1), Ae2 = NAe2 * pMyBox->GetScale().y;
		D3DXVECTOR3 NAe3 = pMyBox->GetDirection(2), Ae3 = NAe3 * pMyBox->GetScale().z;
		D3DXVECTOR3 NBe1 = pOppBox->GetDirection(0), Be1 = NBe1 * pOppBox->GetScale().x;
		D3DXVECTOR3 NBe2 = pOppBox->GetDirection(1), Be2 = NBe2 * pOppBox->GetScale().y;
		D3DXVECTOR3 NBe3 = pOppBox->GetDirection(2), Be3 = NBe3 * pOppBox->GetScale().z;
		D3DXVECTOR3 Interval = pMyBox->GetPosition() - pOppBox->GetPosition();

		FLOAT rA, rB, L;
		auto isHitLength = [&](
		const D3DXVECTOR3& e, 
		D3DXVECTOR3 Ne,
		D3DXVECTOR3 e1, D3DXVECTOR3 e2, D3DXVECTOR3 e3 )
		{
			rA = D3DXVec3Length( &e );
			rB = LenSegOnSeparateAxis( &Ne, &e1, &e2, &e3 );
			L = fabs(D3DXVec3Dot( &Interval, &Ne ));
			if( L > rA + rB ){
				// 衝突していない.
				pMyBox->SetHitOff();
				pOppBox->SetHitOff();
				return false; // 衝突していない.
			}
			return true;
		};

		auto isHitCross = [&]( 
			const D3DXVECTOR3& NAe, const D3DXVECTOR3& NBe, 
			D3DXVECTOR3 Ae1, D3DXVECTOR3 Ae2,
			D3DXVECTOR3 Be1, D3DXVECTOR3 Be2 )
		{
			D3DXVECTOR3 Cross;
			D3DXVec3Cross( &Cross, &NAe, &NBe );
			rA = LenSegOnSeparateAxis( &Cross, &Ae1, &Ae2 );
			rB = LenSegOnSeparateAxis( &Cross, &Be1, &Be2 );
			L = fabs(D3DXVec3Dot( &Interval, &Cross ));
			if( L > rA + rB ){
				// 衝突していない.
				pMyBox->SetHitOff();
				pOppBox->SetHitOff();
				return false; // 衝突していない.
			}
			return true;
		};

		// 分離軸 : Ae.
		if( isHitLength( Ae1, NAe1, Be1, Be2, Be3 ) == false ) return false;
		if( isHitLength( Ae2, NAe2, Be1, Be2, Be3 ) == false ) return false;
		if( isHitLength( Ae3, NAe3, Be1, Be2, Be3 ) == false ) return false;
		// 分離軸 : Be.
		if( isHitLength( Be1, NBe1, Ae1, Ae2, Ae3 ) == false ) return false;
		if( isHitLength( Be2, NBe2, Ae1, Ae2, Ae3 ) == false ) return false;
		if( isHitLength( Be3, NBe3, Ae1, Ae2, Ae3 ) == false ) return false;

		// 分離軸 : C1.
		if( isHitCross( NAe1, NBe1, Ae2, Ae3, Be2, Be3 ) == false ) return false;
		if( isHitCross( NAe1, NBe2, Ae2, Ae3, Be1, Be3 ) == false ) return false;
		if( isHitCross( NAe1, NBe3, Ae2, Ae3, Be1, Be2 ) == false ) return false;
		// 分離軸 : C2.
		if( isHitCross( NAe2, NBe1, Ae1, Ae3, Be2, Be3 ) == false ) return false;
		if( isHitCross( NAe2, NBe2, Ae1, Ae3, Be1, Be3 ) == false ) return false;
		if( isHitCross( NAe2, NBe3, Ae1, Ae3, Be1, Be2 ) == false ) return false;
		// 分離軸 : C3.
		if( isHitCross( NAe3, NBe1, Ae1, Ae2, Be2, Be3 ) == false ) return false;
		if( isHitCross( NAe3, NBe2, Ae1, Ae2, Be1, Be3 ) == false ) return false;
		if( isHitCross( NAe3, NBe3, Ae1, Ae2, Be1, Be2 ) == false ) return false;

		// 衝突している.
		pMyBox->SetHitOn();
		pOppBox->SetHitOn();

		return true;
	}

	//---------------------------------------.
	// レイと球体の当たり判定.
	//---------------------------------------.
	bool IsRayToSphere( CRay* pRay, CSphere* pSphere, D3DXVECTOR3* pOutStartPos, D3DXVECTOR3* pOutEndPos )
	{
		if( pRay	== nullptr ) return false;
		if( pSphere	== nullptr ) return false;

		const D3DXVECTOR3 v = pRay->GetVector();
		const D3DXVECTOR3 l = pRay->GetStartPos();
		const D3DXVECTOR3 p = pSphere->GetPosition() - l;
		const float A = v.x * v.x + v.y * v.y + v.z * v.z;
		const float B = v.x * p.x + v.y * p.y + v.z * p.z;
		const float C = p.x * p.x + p.y * p.y + p.z * p.z - pSphere->GetRadius()*pSphere->GetRadius();

		// ベクトルの長さがなければ終了.
		if( A == 0.0f ){
			pRay->SetHitOff();
			pSphere->SetHitOff();
			return false;
		}
		const float s = B * B - A * C;
		const float sq = sqrtf(s);
		const float a1 = ( B - sq ) / A;
		const float a2 = ( B + sq ) / A;

		// レイの開始位置と球体の距離が.
		//	球体の半径より小さければ衝突している.
		if( D3DXVec3Length( &p ) <= pSphere->GetRadius() ){
			*pOutStartPos = pRay->GetStartPos();

			pOutEndPos->x = l.x + a2 * v.x;
			pOutEndPos->y = l.y + a2 * v.y;
			pOutEndPos->z = l.z + a2 * v.z;

			pRay->SetHitOn();
			pSphere->SetHitOn();
			return true;
		}

		if( s < 0.0f ){
			pRay->SetHitOff();
			pSphere->SetHitOff();
			return false;
		}

		if( a1 < 0.0f || a2 < 0.0f ){
			pRay->SetHitOff();
			pSphere->SetHitOff();
			return false;
		}

		pRay->SetHitOn();
		pSphere->SetHitOn();

		if( pOutStartPos == nullptr ) return true;
		pOutStartPos->x = l.x + a1 * v.x;
		pOutStartPos->y = l.y + a1 * v.y;
		pOutStartPos->z = l.z + a1 * v.z;

		if( pOutEndPos == nullptr ) return true;
		pOutEndPos->x = l.x + a2 * v.x;
		pOutEndPos->y = l.y + a2 * v.y;
		pOutEndPos->z = l.z + a2 * v.z;

		return true;
	}

	//---------------------------------------.
	// レイとメッシュの当たり判定.
	//---------------------------------------.
	bool IsRayToMesh( CRay* pRay, CMesh* pMesh, float* pOutDistance, D3DXVECTOR3* pIntersect, D3DXVECTOR3* pOutNormal, const bool& isNormalHit )
	{
		pRay->SetHitOff();	// ヒットフラグをおろしとく.

		// 対象メッシュのの逆行列を求める.
		D3DXMATRIX mInvWorld;
		D3DXMatrixInverse( &mInvWorld, nullptr, &pMesh->GetTranceform().GetWorldMatrix() );

		// レイの開始位置と終了位置を設定.
		D3DXVECTOR3 startVec, endVec;
		startVec	= pRay->GetStartPos();
		endVec		= pRay->GetStartPos() + (pRay->GetVector()*pRay->GetLength());

		// レイの始点,終点に反映.
		D3DXVec3TransformCoord( &startVec, &startVec, &mInvWorld );
		D3DXVec3TransformCoord( &endVec, &endVec, &mInvWorld );

		// 方向を求める(ベクトル).
		D3DXVECTOR3 vecDirection = endVec - startVec;

		BOOL		isHit	= FALSE;	// 命中フラグ.
		DWORD		dwIndex	= 0;		// インデックス番号.
		D3DXVECTOR3	vertex[3];			// 頂点座標.
		FLOAT		U = 0.0f, V = 0.0f;	// 重心ヒット座標.

		// 対象メッシュとレイとが当たっているか判定.
		D3DXIntersect(
			pMesh->GetMesh(),		// 対象メッシュ.
			&startVec,				// レイの開始位置.
			&vecDirection,			// レイの方向.
			&isHit,					// (out)判定結果.
			&dwIndex,				// (out)衝突時にレイの始点に最も近い面のインデックス番号.
			&U, &V,					// (out)重心ヒット座標.
			pOutDistance,			// (out)メッシュとの距離.
			nullptr, nullptr );

		// 当たってないので終了.
		if( isHit == FALSE ) return false;

		// 頂点情報を取得できなかったので終了.
		if( FAILED( pMesh->FindVerticesOnPoly( dwIndex, vertex )) ) return false;

		// ローカル交点pは、v0 + U*(v1-v0) + v*(v2-v0) で求まる.
		D3DXVECTOR3 v0, v1, v2;
		v0 = vertex[0];
		v1 = vertex[1];
		v2 = vertex[2];
		*pIntersect = v0 + U * (v1 - v0) + V * (v2 - v0);

		// メッシュとの距離が '1,0' より大きければ終了.
		if( *pOutDistance > 1.0f ) return false;

		if( isNormalHit == true ){
			D3DXVECTOR3 normal;
			// 二本のベクトルに対して、直角のベクトルを作成する.
			D3DXVec3Cross( &normal, &(v1 - v0), &(v2 - v0) );
			D3DXVec3Normalize( &normal, &normal );

			if( pOutNormal != nullptr ) *pOutNormal = normal;

			// vecDirectionとvNormalが逆向きの時DOT演算の結果が-1、
			// 一緒だったら１、逆向きの時はレイとレイが対面でぶつかり合っているから当たっている、
			// 同じ向きの時は対面でぶつかっていないから当たっていない.
			float vecDirDotNormal = D3DXVec3Dot( &vecDirection, &normal );
			if( vecDirDotNormal >= 0.0f ) return false;

			// 表から来たレイと衝突している.
		}

		// ヒットフラグを立てる.
		pRay->SetHitOn();

		return true;
	}
};