#ifndef EKFGEO_EKF_H
#define EKFGEO_EKF_H


#include "measurement_package.h"
#include "Eigen/Dense"
#include <vector>
#include <string>
#include <fstream>
#include <time.h>


class EKF_GEO {
	/*CTRVģ�ͣ�x��y������ٶ����ٶ��Լ�������������Ը���ǰ����״̬
	* x,y,
	* v ׷�ٳ���������Գ����ٶ�,
	* �� ƫ���ǣ���׷�ٵ�Ŀ�공���ڵ�ǰ��������ϵ����Y�ᣨ����ǰ���򣩵ļн�)
	* �� ƫ�����ٶ�
	*/

public:

	EKF_GEO();

	virtual ~EKF_GEO();
	void initial();

	void ProcessMeasurement(const MeasurementPackage &meas_package);

	/*״̬ת�ƺ���*/
	void StateTransition(double delta_t);
	/*��������Э�������*/
	void ProcessQMatrix(double delta_t);
	/*����״̬ת�ƾ�����ſ˱Ⱦ���*/
	void ProcessJAMatrix(double delta_t);

	/*������ײ��״�ӳ������Ӧ���ſ˱Ⱦ���*/
	Eigen::VectorXd ProcessHJMatrix();

	void Update(const Eigen::VectorXd &z);


	void UpdateEKF(const Eigen::VectorXd &z);
	void Predict(double delta_t);
	void getState(Eigen::VectorXd& x);
	double control_psi(double psi);
private:
	//�ж��Ƿ񱻳�ʼ��
	bool is_initialized_;

	// ��һ����ʱ���
	double  previous_timestamp_;
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
	Eigen::MatrixXd H_laser_;//�����״�ӳ�����
	Eigen::MatrixXd HJ_;//���ײ��״�ӳ������Ӧ���ſ˱Ⱦ���
	// ֱ�߼��ٶ����� m/s^2
	double std_a_;
	// ƫ���Ǽ��ٶ����� rad/s^2
	double std_yawdd_;
};
#endif 
