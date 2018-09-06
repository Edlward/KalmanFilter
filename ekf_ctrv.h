#ifndef EKFCTRV_EKF_H
#define EKFCTRV_EKF_H


#include "measurement_package.h"
#include "Eigen/Dense"
#include <vector>
#include <string>
#include <fstream>
#include <time.h>
#include "kf.h"

class EKF_CTRV {
	/*CTRVģ�ͣ�x��y������ٶ����ٶ��Լ�������������Ը���ǰ����״̬
	* x,y,
	* v ׷�ٳ���������Գ����ٶ�,
	* �� ƫ���ǣ���׷�ٵ�Ŀ�공���ڵ�ǰ��������ϵ����Y�ᣨ����ǰ���򣩵ļн�),�ڳ�������ϵ�µķ�λ��
	* �� ƫ�����ٶ�
	*/

public:
	/**
	* Constructor
	*/
	EKF_CTRV();

	/**
	* Destructor
	*/
	virtual ~EKF_CTRV();

	/**
	* ProcessMeasurement
	* @param meas_package The latest measurement data of either radar or laser
	*/
	void ProcessMeasurement(const MeasurementPackage &meas_package);

	/*״̬ת�ƺ���*/
	void StateTransition(double delta_t);
	/*��������Э�������*/
	void ProcessQMatrix(double delta_t);
	/*����״̬ת�ƾ�����ſ˱Ⱦ���*/
	void ProcessJAMatrix(double delta_t);

	/*������ײ��״�ӳ������Ӧ���ſ˱Ⱦ���*/
	Eigen::VectorXd ProcessHJMatrix();
	/**
	* Updates the state by using standard Kalman Filter equations
	* @param z The measurement at k+1
	*/
	void Update(const Eigen::VectorXd &z);

	/**
	* Updates the state by using Extended Kalman Filter equations
	* @param z The measurement at k+1
	*/
	void UpdateEKF(const Eigen::VectorXd &z);
	void Predict(double delta_t);
	void getState(Obj &Obstacles);
	void getState(Eigen::VectorXd& x);
private:
	//�ж��Ƿ񱻳�ʼ��
	bool is_initialized_;

	// ��һ����ʱ���
	long long  previous_timestamp_;
	///* ״̬����
	Eigen::VectorXd x_;

	///* ״̬x_Э�������ϵͳ�Ĳ�ȷ���̶�
	Eigen::MatrixXd P_;

	// ״̬ת�ƺ���
	//Eigen::MatrixXd F_;
	Eigen::MatrixXd JA_;

	//����������Э�������
	Eigen::MatrixXd Q_;

	//��������
	Eigen::MatrixXd H_;

	// ����Э������󣬱�ʾ�����Ĳ�ȷ���ȣ�ͨ���ɴ����������ṩ
	Eigen::MatrixXd R_;

	Eigen::MatrixXd R_laser_;//�����״��������
	Eigen::MatrixXd R_radar_;//���ײ��״��������
	Eigen::MatrixXd R_laser_radar_;//���ײ��״��������
	Eigen::MatrixXd H_laser_;//�����״�ӳ�����

	Eigen::MatrixXd HJ_;//���ײ��״�ӳ������Ӧ���ſ˱Ⱦ���
	Eigen::MatrixXd HJ_LR;//���ײ��״�+�����״�ӳ������Ӧ���ſ˱Ⱦ���

	// ֱ�߼��ٶ����� m/s^2
	float std_a_;

	// ƫ���Ǽ��ٶ����� rad/s^2
	float std_yawdd_;
};


#endif 
