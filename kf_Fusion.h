#ifndef KF_KF_FUSION_H
#define KF_KF_FUSION_H


#include "measurement_package.h"
#include "Eigen/Dense"
#include <vector>
#include <string>
#include <fstream>
#include "kf.h"



//ֻ�������Ի��Ľ��
class KF_FUSION {
	//���������Ϊ�����״����ݡ����ײ����ݣ�ת�ѿ�������ϵ���Լ�����+���ײ��״ת�ѿ�������ϵ������
public:
    /**
     * Constructor
     */
	KF_FUSION();

    /**
     * Destructor
     */
	virtual ~KF_FUSION();

    /**
     * ProcessMeasurement
     * @param meas_package The latest measurement data of either radar or laser
     */
    void ProcessMeasurement(const MeasurementPackage &meas_package);

	//�������˲�������
	KF ekf_;
	void getState(Eigen::VectorXd& x);
	void initial();
private:
	//�ж��Ƿ񱻳�ʼ��
	bool is_initialized_;

	// ��һ����ʱ���
	long long previous_timestamp_;
	
	Eigen::MatrixXd R_laser_;//�����״��������
	Eigen::MatrixXd R_radar_;//���ײ��״��������
	Eigen::MatrixXd R_laser_radar_;//���ײ��״��������
	Eigen::MatrixXd H_laser_;//�����״�ӳ�����
	Eigen::MatrixXd H_radar_;//���ײ��״�ӳ�����
	Eigen::MatrixXd H_laser_radar_;//���ײ��״�ӳ�����
};


#endif //EKF_EKF_H
