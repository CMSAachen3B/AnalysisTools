/*
 * Objects.h
 *
 *  Created on: Jan 19, 2015
 *      Author: kargoll
 *
 *  The namespace "objects" holds classes which provide object-like access to Ntuple information
 */

#ifndef OBJECTS_H_
#define OBJECTS_H_

#include "Math/Vector3D.h"
#include "TMatrixD.h"
#include "Ntuple_Controller.h"

namespace objects {

typedef ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<double> > Vector3D;

class MET {
public:
	MET();
	MET(Ntuple_Controller* const Ntp, TString met_type);
	virtual ~MET();

	float et() const {return et_;}
	void set_et(float et) {et_ = et;}

	float ex() const {return ex_;}
	void set_ex(float ex) {ex_ = ex;}

	float ey() const {return ey_;}
	void set_ey(float ey) {ey_ = ey;}

	float phi() const {return phi_;}
	void set_phi(float phi) {phi_ = phi;}

	float significance() const {return significance_;}
	void set_significance(float significance) {significance_ = significance;}

	float significanceXX() const {return significanceXX_;}
	void set_significanceXX(float significanceXx) {significanceXX_ = significanceXx;}

	float significanceXY() const {return significanceXY_;}
	void set_significanceXY(float significanceXy) {significanceXY_ = significanceXy;}

	float significanceYY() const {return significanceYY_;}
	void set_significanceYY(float significanceYy) {significanceYY_ = significanceYy;}

	bool hasSignificance() const {return hasSignificance_;}
	void set_hasSignificance(bool hasSignificance) {hasSignificance_ = hasSignificance;}

	TMatrixD significanceMatrix() const;
	Vector3D met3D() const;

private:
	float et_;
	float phi_;
	float ex_;
	float ey_;
	float significance_;
	float significanceXX_;
	float significanceXY_;
	float significanceYY_;

	bool hasSignificance_;
};

} /* namespace objects */
#endif /* OBJECTS_H_ */