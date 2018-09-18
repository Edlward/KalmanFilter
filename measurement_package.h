#ifndef MEASUREMENT_PACKAGE_H_
#define MEASUREMENT_PACKAGE_H_

#include "Eigen/Dense"
const double DoublePI = 2 * M_PI;
//Ŀ��������Դ�Ƕ�Ԫ����,��ͬ��λ�����롢�߶��£�Ŀ�겻�ᱻ���д�����̽�⵽,��ö�ٱ����ϰ����ں����õ��Ĵ�������Ϣ
enum ScanningAttributes  //������Դ
{
	SA_Lidar = 1,	//ֻʹ��Lidar̽�⵽Ŀ�������
	SA_Radar = 2,	//ֻʹ��Radar̽�⵽Ŀ�������
	SA_Camera = 3,	//ֻʹ��Camera̽�⵽Ŀ�������
	SA_Lidar_Radar = 4,		//Lidar+Radar�ں�
	SA_Lidar_Camera = 5,	//Lidar+Camera�ں�
	SA_Radar_Camera = 6,	//Radar+Camera�ں�
	SA_Lidar_Radar_Camera = 7,	//Lidar+Radar+Camera�ں�
};

//�ϰ��Ŀ�꣩��Ϣ
typedef struct Obj
{
	//1-Ŀ�������Ϣ///////////////////////////////////
	short index;                //Ŀ�������Ϣ������Ŀ��
	ScanningAttributes ObstacleAttriSrc;  //�ϰ������Դ�����ʶ����Դ

	float Direction_Local;	//����еĺ����,�ֲ�����ϵ�£�Ŀ����˶�������ǰΪ0��˳ʱ������ 0-360�㣩
	float Direction_Global;	//����еĺ����,�������ϵ�£�Ŀ����˶���������Ϊ0������90�㣩

	//2-�ֲ�����ϵ����������ϵ��Ŀ����Ϣ////////////////  ��������λ��Ϊ����ϵ������ΪX����ǰΪY������ΪZ������λΪm
	double  CenterPoint_Local_x;  //���ĵ�����
	double  CenterPoint_Local_y;  //���ĵ�����
	double ObjSpeed_Local_x;
	double ObjSpeed_Local_y;
	float	Turn_Rate;	//Ŀ��ת��
	float	Acceleration_Local;	//���ٶȣ��غ���

	//���ײ�������Ϣ
	double Range;
	double RelativeVelocity;
	double RelativeAngle;

	//3-Ŀ��ʱ����Ϣ/////////////////////////////
	int	Type_Probs;  //����ɿ��ԣ����Ŷȣ�
	double CurrentTime;	//��ǰ֡��ɨ��ʱ��ʱ�䣨�������һ�γ��ֵ�ʱ�䣩--UTCʱ��

}Obj;

class MeasurementPackage {
public:
  long timestamp_;

  enum SensorType{
    LASER,
	LASER_RADAR,
    RADAR
  } sensor_type_;

  Eigen::VectorXd raw_measurements_;
  MeasurementPackage(){};
  //MeasurementPackage(Obj &Obstacles)
  //{
	 // if (Obstacles.ObstacleAttriSrc == SA_Lidar) {
		//  // LASER MEASUREMENT

		//  // read measurements at this timestamp
		//  sensor_type_ = MeasurementPackage::LASER;
		//  raw_measurements_ = Eigen::VectorXd(2);
		//  raw_measurements_ << Obstacles.CenterPoint_Local_x, Obstacles.CenterPoint_Local_y;
		//  timestamp_ = Obstacles.CurrentTime/**100000*/;
	 // }
	 // else if (Obstacles.ObstacleAttriSrc == SA_Radar) {
		//  // RADAR MEASUREMENT
		//  sensor_type_ = MeasurementPackage::RADAR;
		//  raw_measurements_ = Eigen::VectorXd(3);
		//  raw_measurements_ << Obstacles.Range, Obstacles.RelativeAngle, Obstacles.RelativeVelocity;
		//  timestamp_ = Obstacles.CurrentTime /** 100000*/;
	 // }
  //}
};

#endif /* MEASUREMENT_PACKAGE_H_ */
