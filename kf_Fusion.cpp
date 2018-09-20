#include "kf_Fusion.h"
#include <iostream>
#include <time.h>

/**
 * Initializes  Kalman filter
 */
KF_FUSION::KF_FUSION() {
	is_initialized_ = false;
	previous_timestamp_ = 0;


	H_laser_ = Eigen::MatrixXd(2,4);
	H_laser_ << 1, 0, 0, 0, 
		0, 1, 0, 0;
	H_radar_ = Eigen::MatrixXd(4, 4);
	H_radar_ << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;
	H_laser_radar_ = Eigen::MatrixXd(4, 4);
	H_laser_radar_ << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;
	initial();
	ekf_.F_ = Eigen::MatrixXd(4, 4);
	ekf_.F_ << 1, 0, 1, 0,
		0, 1, 0, 1,
		0, 0, 1, 0,
		0, 0, 0, 1;
	ekf_.Q_ = Eigen::MatrixXd(4, 4);
}

KF_FUSION::~KF_FUSION() {}


void KF_FUSION::ProcessMeasurement(const MeasurementPackage &meas_package) {
	clock_t start, finish;
	start = clock();
    if (!is_initialized_) {
		/*
		 * ��һ�β���ʱ��ʼ��״̬����
		 * ����Э�������
		 * ����radar�Ĳ�����Ҫ����Ӽ�����ת��Ϊ�ѿ�������ϵ
		 */
		ekf_.x_ = Eigen::VectorXd(4);
		ekf_.x_ << 1, 1, 1, 1;

        if (meas_package.sensor_type_ == MeasurementPackage::LASER) {
			ekf_.x_[0] = meas_package.raw_measurements_[0];
			ekf_.x_[1] = meas_package.raw_measurements_[1];
			ekf_.x_[2] = 5;
			ekf_.x_[3] = 0;
		}
		else {
			ekf_.x_[0] = meas_package.raw_measurements_[0];
			ekf_.x_[1] = meas_package.raw_measurements_[1];
			ekf_.x_[2] = meas_package.raw_measurements_[2];
			ekf_.x_[3] = meas_package.raw_measurements_[3];
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
	float delta_t2 = delta_t*delta_t;
	float delta_t3 = delta_t2*delta_t;
	float delta_t4 = delta_t3*delta_t;
	//����F_����
	ekf_.F_(0, 2) = delta_t;
	ekf_.F_(1, 3) = delta_t;
	//����Q_����
	ekf_.Q_ = Eigen::MatrixXd(4,4);
	float noise_ax2 = 9.0;
	float noise_ay2 = 9.0;
	ekf_.Q_ << delta_t4 / 4 * noise_ax2, 0, delta_t3 / 2 * noise_ax2, 0,
				0,   delta_t4 / 4 * noise_ay2, 0, delta_t3 / 2 * noise_ay2,
			   delta_t3 / 2 * noise_ax2, 0, delta_t2*noise_ax2, 0,
		        0,   delta_t3 / 2 * noise_ay2, 0, delta_t2*noise_ay2;
	//����Ԥ��
	ekf_.Predict();
	/*
	 * ����
	 * ���ڴ�����������ѡ����µĲ���
	 */
    if (meas_package.sensor_type_ == MeasurementPackage::RADAR) {
		//radar�ĸ���
		ekf_.H_ = H_radar_;
		ekf_.R_ = R_radar_;//����������
		ekf_.Update(meas_package.raw_measurements_);//����radar���ݲ�����չ�������˲�����
    } else if (meas_package.sensor_type_ == MeasurementPackage::LASER) {
		//lidar�ĸ���
		ekf_.H_ = H_laser_;
		ekf_.R_ = R_laser_;//����������
		ekf_.Update(meas_package.raw_measurements_);//����lidar���ݲ������Կ������˲�����
	}
	else if (meas_package.sensor_type_ == MeasurementPackage::LASER_RADAR) {
		//lidar�ĸ���
		ekf_.H_ = H_laser_radar_;
		ekf_.R_ = R_laser_radar_;//����������
		ekf_.Update(meas_package.raw_measurements_);//����Ĭ��Ϊ����ģ��
	}
	/*
	 * ��ɸ��£�����ʱ��
	 */
	previous_timestamp_ = meas_package.timestamp_;
	finish = clock();
	float totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	//std::cout << " -------------Current Frame cluster Time:" << totaltime*1000.0 << "ms" << std::endl;
}

void KF_FUSION::getState(Eigen::VectorXd& x)
{
	x[0] = ekf_.x_[0];
	x[1] = ekf_.x_[1];
	x[2] = ekf_.x_[2];
	x[3] = ekf_.x_[3];
}

void KF_FUSION::initial()
{
	std::string in_file_name_ = "../config.txt";
	std::ifstream in_file_(in_file_name_.c_str(), std::ifstream::in);
	if (!in_file_.is_open()) {
		std::cerr << "Cannot open input file: " << in_file_name_ << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string line;
	float std_laspx_ = 0.05;
	float std_laspy_ = 0.05;
	float std_radpx_ = 0.3;
	float std_radpy_ = 0.3;
	float std_vx_ = 1.3;
	float std_vy_ = 1.3;

	float px_ = 1;
	float py_ = 1;
	float pvx_ = 0.5;
	float pvy_ = 0.5;
	while (getline(in_file_, line))
	{
		std::istringstream iss(line);
		std::string data_type;
		iss >> data_type;
		if (data_type.compare("std_laspx_") == 0) {
			iss >> std_laspx_;
			continue;
		}
		if (data_type.compare("std_laspy_") == 0) {
			iss >> std_laspy_;
			continue;
		}
		if (data_type.compare("std_radpx_") == 0) {
			iss >> std_radpx_;
			continue;
		}
		if (data_type.compare("std_radpy_") == 0) {
			iss >> std_radpy_;
			continue;
		}
		if (data_type.compare("std_vx_") == 0) {
			iss >> std_vx_;
			continue;
		}
		if (data_type.compare("std_vy_") == 0) {
			iss >> std_vy_;
			continue;
		}
		if (data_type.compare("px_") == 0) {
			iss >> px_;
			continue;
		}
		if (data_type.compare("py_") == 0) {
			iss >> py_;
			continue;
		}
		if (data_type.compare("pvx_") == 0) {
			iss >> pvx_;
			continue;
		}
		if (data_type.compare("pvy_") == 0) {
			iss >> pvy_;
			continue;
		}
	}
	R_laser_ = Eigen::MatrixXd(2, 2);
	R_laser_ << std_laspx_*std_laspx_, 0,
		0, std_laspy_*std_laspy_;
	R_radar_ = Eigen::MatrixXd(4, 4);
	R_radar_ << std_radpx_*std_radpx_, 0, 0, 0,
		0, std_radpy_*std_radpy_, 0, 0,
		0, 0, std_vx_*std_vx_, 0,
		0, 0, 0, std_vy_*std_vy_;

	R_laser_radar_ = Eigen::MatrixXd(4, 4);
	R_laser_radar_ << std_laspx_*std_laspx_, 0, 0, 0,
		0, std_laspy_*std_laspy_, 0, 0,
		0, 0, std_vx_*std_vx_, 0,
		0, 0, 0, std_vx_*std_vx_;

	//ϵͳ״̬��ȷ����
	ekf_.P_ = Eigen::MatrixXd(4, 4);
	ekf_.P_ << px_, 0, 0, 0,
		0, py_, 0, 0,
		0, 0, pvx_, 0,
		0, 0, 0, pvy_;

	//״̬����
	ekf_.x_ = Eigen::VectorXd(4);
}
