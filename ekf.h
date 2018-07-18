#ifndef EKF_EKF_H
#define EKF_EKF_H


#include "measurement_package.h"
#include "Eigen/Dense"
#include <vector>
#include <string>
#include <fstream>
#include "kf.h"

class EKF {
public:
    /**
     * Constructor
     */
    EKF();

    /**
     * Destructor
     */
    virtual ~EKF();

    /**
     * ProcessMeasurement
     * @param meas_package The latest measurement data of either radar or laser
     */
    void ProcessMeasurement(const MeasurementPackage &meas_package);

	//�������˲�������
	KF ekf_;

private:
	//�ж��Ƿ񱻳�ʼ��
	bool is_initialized_;

	// ��һ����ʱ���
	long long previous_timestamp_;
	
	Eigen::MatrixXd R_laser_;//�����״��������
	Eigen::MatrixXd R_radar_;//���ײ��״��������
	Eigen::MatrixXd H_laser_;//�����״�ӳ�����
	Eigen::MatrixXd Hj_;//���ײ��״�ӳ������Ӧ���ſ˱Ⱦ���
};


#endif //EKF_EKF_H
