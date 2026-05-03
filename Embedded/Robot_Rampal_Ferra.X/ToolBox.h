/* 
 * File:   ToolBox.h
 * Author: E306-PC2
 *
 * Created on 10 septembre 2025, 09:51
 */

#ifndef TOOLBOX_H
#define	TOOLBOX_H
#define PI 3.141592653589793
float Abs(float value);
float Max(float value, float value2);
float Min(float value, float value2);
float LimitToInterval(float value, float lowLimit, float highLimit);
float RadianToDegree(float value);
float DegreeToRadian(float value);
float getFloatFromBytes(const unsigned char *p, int index);


#endif	/* TOOLBOX_H */

