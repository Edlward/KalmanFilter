#ifndef KF_KF_H
#define KF_KF_H


#include "measurement_package.h"
#include "Eigen/Dense"
#include <vector>
#include <string>
#include <fstream>
#include <math.h>

class KF {
public:

    ///* ״̬����
	Eigen::VectorXd x_;

    ///* ״̬x_Э�������ϵͳ�Ĳ�ȷ���̶�
	Eigen::MatrixXd P_;

	// ״̬ת�ƺ���
	Eigen::MatrixXd F_;

	//Э�������
	Eigen::MatrixXd Q_;
	
	//��������
	Eigen::MatrixXd H_;

	// ����Э������󣬱�ʾ�����Ĳ�ȷ���ȣ�ͨ���ɴ����������ṩ
	Eigen::MatrixXd R_;

    /**
     * Constructor
     */
    KF();

    /**
     * Destructor
     */
    virtual ~KF();

	/**
	* ��ʼ�� Kalman filter
	* @param x_in Initial state
	* @param P_in Initial state covariance
	* @param F_in Transition matrix
	* @param H_in Measurement matrix
	* @param R_in Measurement covariance matrix
	* @param Q_in Process covariance matrix
	*/
	void Init(Eigen::VectorXd &x_in, Eigen::MatrixXd &P_in, Eigen::MatrixXd &F_in,
		Eigen::MatrixXd &H_in, Eigen::MatrixXd &R_in, Eigen::MatrixXd &Q_in);
	/**
	* Prediction Predicts the state and the state covariance
	* using the process model
	* @param delta_T Time between k and k+1 in s
	*/
	void Predict();

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

	/**
	* Updates the state by using Extended Kalman Filter equations
	* @param y The difference between measurement and predicted state at k+1
	*/
	void KalmanFilter(const Eigen::VectorXd &y);
};


#endif //EKF_EKF_H
