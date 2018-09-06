#include "ekf_ctrv.h"
#include <iostream>


EKF_CTRV::EKF_CTRV() {
	is_initialized_ = false;
	previous_timestamp_ = 0;


	H_laser_ = Eigen::MatrixXd(2, 5);
	H_laser_ << 1, 0, 0, 0, 0,
		0, 1, 0, 0, 0;
	float std_laspx_ = 0.15;
	float std_laspy_ = 0.15;
	R_laser_ = Eigen::MatrixXd(2, 2);
	/*����ֵ�Ĳ�ȷ����*/
	R_laser_ << std_laspx_*std_laspx_, 0,
		0, std_laspy_*std_laspy_;


	float std_radrho_ = 0.3;
	float std_radphi_ = 0.03;
	float std_radrhodot_ = 0.3;
	R_radar_ = Eigen::MatrixXd(3, 3);
	/*����ֵ�Ĳ�ȷ����*/
	R_radar_ << std_radrho_*std_radrho_, 0, 0,
		0, std_radphi_*std_radphi_, 0,
		0, 0, std_radrhodot_*std_radrhodot_;


	//״̬����
	x_ = Eigen::VectorXd(5);
	//ϵͳ״̬��ȷ����
	P_ = Eigen::MatrixXd(5, 5);
	P_ << 1, 0, 0, 0, 0,
		0, 1, 0, 0, 0,
		0, 0, 1, 0, 0,
		0, 0, 0, 1, 0,
		0, 0, 0, 0, 1;
	/*����Ҫ״̬ת�ƾ���*/
	Q_ = Eigen::MatrixXd(5, 5);//״̬Э�������
	JA_ = Eigen::MatrixXd(5, 5);//״̬ת�ƾ�����ſ˱Ⱦ���
	HJ_ = Eigen::MatrixXd(3, 5);//Ԥ��ռ䵽�����ռ���ſ˱Ⱦ���
	std_a_ = 1.0;
	std_yawdd_ = 0.5;
}

EKF_CTRV::~EKF_CTRV() {}
/**
* \brief ����CTRVģ�͵�״̬ת�ƺ����Ƿ�����ģ�ͣ���Ҫֱ�ӽ��м��㣬������ͨ��״̬ת�Ʒ��̼���
*/
void EKF_CTRV::StateTransition(double delta_t)
{
	float x = x_[0];
	float y = x_[1];
	float v = x_[2];
	float theta = x_[3];
	float omiga = x_[4];
	if (omiga > 0.00001)
	{
		float tmpTheta = 0, v_omiga = v / omiga;
		x_[2] = v;								   //����ٶ�
		x_[3] = tmpTheta = omiga*delta_t + theta;   //ƫ����
		x_[4] = omiga;							   //ƫ�����ٶ�

		x_[0] = v_omiga*sin(tmpTheta) - v_omiga*sin(theta) + x;//x
		x_[1] = -v_omiga*cos(tmpTheta) + v_omiga*cos(theta) + y;//y
	}
	else
	{
		x_[2] = v;								   //����ٶ�
		x_[3] = omiga*delta_t + theta;             //ƫ����
		x_[4] = omiga;							   //ƫ�����ٶ�

		x_[0] = v*cos(theta)*delta_t + x;//x
		x_[1] = v*sin(theta)*delta_t + y;//y
	}
//	std::cout <<"x_: "<< x_ << std::endl;
}

void EKF_CTRV::ProcessQMatrix(double delta_t)
{
	float delta_t2 = delta_t*delta_t;
	float delta_t3 = delta_t2*delta_t;
	float delta_t4 = delta_t3*delta_t;
	float std_a_2 = std_a_*std_a_;
	float std_yawdd_2 = std_yawdd_*std_yawdd_;
	float theta = x_[3];
	//����Q_����
	Q_ = Eigen::MatrixXd(5, 5);

	float q11 = 0.25*delta_t4*std_a_2*cos(theta)*cos(theta);
	float q12 = 0.25*delta_t4*std_a_2*sin(theta)*cos(theta);
	float q13 = 0.5*delta_t3*std_a_2*cos(theta);
	float q14 = 0;
	float q15 = 0;

	float q21 = 0.25*delta_t4*std_a_2*sin(theta)*cos(theta);
	float q22 = 0.25*delta_t4*std_a_2*sin(theta)*sin(theta);
	float q23 = 0.5*delta_t3*std_a_2*sin(theta);
	float q24 = 0;
	float q25 = 0;

	float q31 = 0.5*delta_t3*std_a_2*cos(theta);
	float q32 = 0.5*delta_t3*std_a_2*sin(theta);
	float q33 = delta_t2*std_a_2;
	float q34 = 0;
	float q35 = 0;

	float q41 = 0;
	float q42 = 0;
	float q43 = 0;
	float q44 = 0.25*delta_t4*std_yawdd_2;
	float q45 = 0.5*delta_t3*std_yawdd_2;

	float q51 = 0;
	float q52 = 0;
	float q53 = 0;
	float q54 = 0.5*delta_t3*std_yawdd_;
	float q55 = delta_t2*std_yawdd_;
	Q_ << q11, q12, q13, q14, q15,
		q21, q22, q23, q24, q25,
		q31, q32, q33, q34, q35,
		q41, q42, q43, q44, q45,
		q51, q52, q53, q54, q55;
//	std::cout << "Q_: " << Q_ << std::endl;
}

void EKF_CTRV::ProcessJAMatrix(double delta_t)
{
	float v = x_[2];
	float theta = x_[3];
	float omiga = x_[4];
	float tmpTheta = omiga*delta_t + theta;
	float v_omiga = v / omiga;
	float _1_omiga = 1 / omiga;
	float delta_t_v_omiga = delta_t*v_omiga;
	float v_omiga2 = v / omiga / omiga;
	JA_ = Eigen::MatrixXd(5, 5);//״̬ת�ƾ�����ſ˱Ⱦ���

	float q11, q12, q13, q14, q15, q21, q22, q23, q24, q25;
	if (omiga > 0.00001)
	{
		q11 = 1;
		q12 = 0;
		q13 = _1_omiga*(-sin(theta) + sin(tmpTheta));
		q14 = v_omiga*(-cos(theta) + cos(tmpTheta));
		q15 = delta_t_v_omiga*cos(tmpTheta) - v_omiga2*(-sin(theta) + sin(tmpTheta));

		q21 = 0;
		q22 = 1;
		q23 = v_omiga*(-sin(theta) + sin(tmpTheta));
		q24 = _1_omiga*(cos(theta) - cos(tmpTheta));;
		q25 = delta_t_v_omiga*sin(tmpTheta) - v_omiga2*(cos(theta) - cos(tmpTheta));
	}
	else
	{
		q11 = 1;
		q12 = 0;
		q13 = delta_t*cos(theta);
		q14 = -delta_t*v*sin(theta);
		q15 = 0;

		q21 = 0;
		q22 = 1;
		q23 = delta_t*sin(theta);
		q24 = delta_t*v*cos(theta);;
		q25 = 0;
	}

	float q31 = 0;
	float q32 = 0;
	float q33 = 1;
	float q34 = 0;
	float q35 = 0;

	float q41 = 0;
	float q42 = 0;
	float q43 = 0;
	float q44 = 1;
	float q45 = delta_t;

	float q51 = 0;
	float q52 = 0;
	float q53 = 0;
	float q54 = 0;
	float q55 = 1;
	JA_ << q11, q12, q13, q14, q15,
		q21, q22, q23, q24, q25,
		q31, q32, q33, q34, q35,
		q41, q42, q43, q44, q45,
		q51, q52, q53, q54, q55;
//	std::cout << "JA_: " << JA_ << std::endl;
}

Eigen::VectorXd EKF_CTRV::ProcessHJMatrix()
{
	Eigen::VectorXd hx = Eigen::VectorXd(3);
	HJ_ = Eigen::MatrixXd(3, 5);//Ԥ��ռ䵽�����ռ���ſ˱Ⱦ���
	float x = x_[0];
	float y = x_[1];
	float v = x_[2];
	float x2y2 = (x*x + y*y);
	float sqrt_x2y2 = (sqrt(x2y2));
	float sqrt23_x2y2 = pow(x2y2, 2.0 / 3.0);
	float vxvy = v*x + v*y;
	float q11 = x / sqrt_x2y2;
	float q12 = y / sqrt_x2y2;
	float q13 = 0;
	float q14 = 0;
	float q15 = 0;

	float q21 = -y / x2y2;
	float q22 = x / x2y2;
	float q23 = 0;
	float q24 = 0;
	float q25 = 0;

	float q31 = v / sqrt_x2y2 - x*vxvy / sqrt23_x2y2;
	float q32 = v / sqrt_x2y2 - y*vxvy / sqrt23_x2y2;
	float q33 = (x + y) / sqrt_x2y2;
	float q34 = 0;
	float q35 = 0;

	HJ_ << q11, q12, q13, q14, q15,
		q21, q22, q23, q24, q25,
		q31, q32, q33, q34, q35;
	hx << sqrt_x2y2, atan2(y, x), vxvy / sqrt_x2y2;
	return hx;
}

void EKF_CTRV::Predict(double delta_t)
{
	/*״̬ת��*/
	StateTransition(delta_t);
	/*����Q��*/
	ProcessQMatrix(delta_t);
	/*����JA����*/
	ProcessJAMatrix(delta_t);
	/*����Ԥ��*/
	P_ = JA_*P_*JA_.transpose() + Q_;
}

void EKF_CTRV::getState(Obj& Obstacles)
{
	Obstacles.CenterPoint_Local_x = x_[0];
	Obstacles.CenterPoint_Local_y = x_[1];
	Obstacles.ObjSpeed_Local_x = x_[2];
	Obstacles.ObjSpeed_Local_y = x_[3];
}

void EKF_CTRV::getState(Eigen::VectorXd& x)
{
	x[0] = x_[0];
	x[1] = x_[1];
	x[2] = x_[2];
	x[3] = x_[3];
}

void EKF_CTRV::Update(const Eigen::VectorXd &z)
{
	//���ڿ�������״̬���и���
	Eigen::VectorXd z_pred = H_*x_;//״̬�ռ䵽�����ռ��ת��
	Eigen::VectorXd y = z - z_pred;//��ȡ����ֵ��״ֵ̬֮��Ĳ�

	Eigen::MatrixXd HT = H_.transpose();
	Eigen::MatrixXd S = H_*P_*HT + R_;
	Eigen::MatrixXd Si = S.inverse();
	Eigen::MatrixXd PHT = P_*HT;
	Eigen::MatrixXd K = PHT*Si;

	//״̬����
	x_ = x_ + K*y;
	long x_size = x_.size();
	Eigen::MatrixXd I = Eigen::MatrixXd::Identity(x_size, x_size);
	P_ = (I - K*H_)*P_;
}

void EKF_CTRV::UpdateEKF(const Eigen::VectorXd &z)
{
	//���ڿ�������״̬���и���
	Eigen::VectorXd z_pred = ProcessHJMatrix();//״̬�ռ䵽�����ռ��ת��
	Eigen::VectorXd y = z - z_pred;//��ȡ����ֵ��״ֵ̬֮��Ĳ�


	Eigen::MatrixXd HJ_T = HJ_.transpose();
	Eigen::MatrixXd S = HJ_*P_*HJ_T + R_;
	Eigen::MatrixXd Si = S.inverse();
	Eigen::MatrixXd PHT = P_*HJ_T;
	Eigen::MatrixXd K = PHT*Si;

	//״̬����
	x_ = x_ + K*y;
	long x_size = x_.size();
	Eigen::MatrixXd I = Eigen::MatrixXd::Identity(x_size, x_size);
	P_ = (I - K*HJ_)*P_;
}

/**
* @param {MeasurementPackage} meas_package The latest measurement data of
* either radar or laser.
*/
void EKF_CTRV::ProcessMeasurement(const MeasurementPackage &meas_package) {
	clock_t start, finish;
	start = clock();
	if (!is_initialized_)
	{
		/*
		* ��һ�β���ʱ��ʼ��״̬����
		* ����radar�Ĳ�����Ҫ����Ӽ�����ת��Ϊ�ѿ�������ϵ
		*/
		// first measurement
		x_ = Eigen::VectorXd(5);
		x_ << 0, 0, 0, 0, 0;

		if (meas_package.sensor_type_ == MeasurementPackage::LASER)
		{
			//std::cout << "laser data: " << std::endl;
			x_[0] = meas_package.raw_measurements_[0];//x
			x_[1] = meas_package.raw_measurements_[1];//y
			x_[2] = 0;								   //����ٶ�
			x_[3] = 0;								   //ƫ����
			x_[4] = 0;								   //ƫ�����ٶ�
		}
		else if (meas_package.sensor_type_ == MeasurementPackage::RADAR){
			//std::cout << "radar data: " << std::endl;
			float rho = meas_package.raw_measurements_[0];
			float phi = meas_package.raw_measurements_[1];
			float rho_dot = meas_package.raw_measurements_[2];
			x_[0] = rho * cos(phi);
			x_[1] = rho * sin(phi);
			x_[2] = rho_dot;
			x_[3] = 0;
			x_[4] = 0;
		}
		else
		{
			x_[0] = meas_package.raw_measurements_[0];
			x_[1] = meas_package.raw_measurements_[1];
			x_[2] = meas_package.raw_measurements_[4];
			x_[3] = 0;
			x_[4] = 0;
		}
		previous_timestamp_ = meas_package.timestamp_;
		is_initialized_ = true;
		return;
	}
	/*
	* ����ʱ�����״̬ת������F_
	* ʱ����sΪ��λ
	* ���´���������Э�������
	*/
	double delta_t = (double(meas_package.timestamp_) - double(previous_timestamp_)) / 1000000.0;

	//1.����Ԥ��--------------------------------------------------------
	Predict(delta_t);

	//2.����״̬����--------------------------------------------------------
	/*
	* ����
	* ���ڴ�����������ѡ����µĲ���
	*/
	if (meas_package.sensor_type_ == MeasurementPackage::RADAR) {
		//radar�ĸ���
		R_ = R_radar_;//����������
		UpdateEKF(meas_package.raw_measurements_);//����radar���ݲ�����չ�������˲�����
	}
	else if (meas_package.sensor_type_ == MeasurementPackage::LASER_RADAR)
	{
		//lidar�ĸ���
		H_ = Eigen::MatrixXd(2, 5);//״̬�ռ䵽�����ռ��ӳ�����
		H_ << 1, 0, 0, 0, 0,
			0, 1, 0, 0, 0;
		R_ = R_laser_;//����������
		Eigen::VectorXd raw_measurements_lidar = Eigen::VectorXd(2);
		raw_measurements_lidar << meas_package.raw_measurements_[0], meas_package.raw_measurements_[1];
		Update(raw_measurements_lidar);//����lidar���ݲ������Կ������˲�����

		//radar�ĸ���
		R_ = R_radar_;//����������
		Eigen::VectorXd raw_measurements_radar = Eigen::VectorXd(3);
		raw_measurements_radar << meas_package.raw_measurements_[2], meas_package.raw_measurements_[3], meas_package.raw_measurements_[4];
		UpdateEKF(raw_measurements_radar);//����radar���ݲ�����չ�������˲�����
	}
	else if (meas_package.sensor_type_ == MeasurementPackage::LASER) {
		//lidar�ĸ���
		H_ = Eigen::MatrixXd(2, 5);//״̬�ռ䵽�����ռ��ӳ�����
		H_ << 1, 0, 0, 0, 0,
			0, 1, 0, 0, 0;
		R_ = R_laser_;//����������
		Update(meas_package.raw_measurements_);//����lidar���ݲ������Կ������˲�����
	}
	/*
	* ��ɸ��£�����ʱ��
	*/
	previous_timestamp_ = meas_package.timestamp_;
	finish = clock();
	float totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	//std::cout << " -------------Current Frame cluster Time:" << totaltime*1000.0 << "ms" << std::endl;
}