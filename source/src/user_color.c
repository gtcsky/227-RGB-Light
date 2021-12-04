/*
 * user_color.c
 *
 *  Created on: 2019��9��26��
 *      Author: Bough_004
 */

#include "user_color.h"
#include "user_pca.h"
#include "user_menu.h"
#include "math.h"
//#include "pca.h"
//void hsb2rgb(float h, float s, float v) {

//															//32,    33   34  35        36    37   38        39  40   41      42  43   44   45  46     47   48   49   50   51       52    53   54  55    56
//const u16 colorTempCoolArray[MAX_ColorTemp-MIN_ColorTemp+1]={    0,    190,  250,300,    340, 380,420,      460,500,560,  590,630,660,700,735,   755,800,830,845,880,    900,930, 960,990,1030};
////const u16 colorTempWarmArray[MAX_ColorTemp-MIN_ColorTemp+1]={0,120,  160,190,  260, 330, 390,    410, 450,490,  550,580,620,660,695,  730,760,790, 820,860,  890,925,960,1000,PWM_MAX_CW};
//														     //    32,   33   34  35         36    37   38        39  40   41      42  43   44   45  46      47   48   49   50   51     52    53   54  55    56
//const u16 colorTempWarmArray[MAX_ColorTemp-MIN_ColorTemp+1]={1080,960,   920,890,    820, 750, 690,    670, 630,590,  530,500,460,420,385,   350,320,290, 260,220,  190,155,  120,80   ,0};
															//32,    33   34  35        		36    37   38        39  40   41      42  43   44   45  46     47   48   49   50   51       	52    53   54  55    56
const u16 colorTempCoolArray[MAX_ColorTemp-MIN_ColorTemp+1]={    0,    160,  220,270,    		330, 370,410,      450,500,530,  570,600,650,700,740,   780,810,840,880,900,    	920,970, 1030,1070,1120};
//const u16 colorTempWarmArray[MAX_ColorTemp-MIN_ColorTemp+1]={0,120,  160,190,  			260, 330, 390,    410, 450,490,  550,580,620,660,695,  730,760,790, 820,860,  	890,925,960,1000,PWM_MAX_CW};
														     //    32,   33   34  35         		36    37   38        39  40   41      42  43   44   45  46      47   48   49   50   51     	52    53   54  55    56
const u16 colorTempWarmArray[MAX_ColorTemp-MIN_ColorTemp+1]={1160,1100, 1050,980,   		930, 850, 790,    750, 690,640,  590,550,510,480,450,   410,370,340, 320,280, 	 255,210,  165,140   ,0};

//const float colorChangeTableArray[101]={
//		0,0.020,0.022,0.026,0.030,   		0.034,0.038,0.042,0.048,0.052,
//		0.057,0.061,0.065,0.069,0.073,		0.077,0.081,0.085,0.089,0.093,
//		0.098,0.115,0.120,0.125,0.130,		0.135,0.140,0.145,0.150,0.156,
//		0.162,0.168,0.174,0.180,0.186,		0.193,0.200,0.207,0.214,0.221,
//		0.230,0.239,0.248,0.257,0.266,		0.275,0.284,0.293,0.302,0.311,
//		0.321,0.331,0.341,0.351,0.361,		0.371,0.381,0.391,0.401,0.411,
//		0.423,0.435,0.447,0.459,0.471,		0.483,0.495,0.507,0.520,0.533,
//		0.546,0.559,0.572,0.585,0.598,		0.612,0.626,0.640,0.654,0.669,
//		0.684,0.699,0.714,0.729,0.734,		0.749,0.765,0.770,0.785,0.790,
//		0.810,0.830,0.850,0.870,0.890,		0.910,0.930,0.950,0.975,0.995,1.000
//};

const float saturationArray[101]={
				0,0.015,0.030,0.045,0.060,		0.075,0.090,0.105,0.120,0.135,
				0.150,0.165,0.180,0.195,0.210,		0.225,0.240,0.255,0.270,0.285,
				0.300,0.315,0.330,0.345,0.360,		0.375,0.390,0.405,0.420,0.435,
				0.450,0.465,0.480,0.495,0.510,		0.525,0.540,0.555,0.570,0.585,
				0.600,0.610,0.620,0.630,0.640,		0.650,0.650,0.670,0.680,0.690,
				0.700,0.710,0.720,0.730,0.740,		0.750,0.760,0.770,0.780,0.790,
				0.800,0.805,0.810,0.815,0.820,		0.825,0.830,0.835,0.840,0.845,
				0.850,0.855,0.860,0.865,0.870,		0.875,0.880,0.885,0.890,0.895,
				0.900,0.905,0.910,0.915,0.920,		0.925,0.930,0.935,0.940,0.945,
				0.950,0.955,0.960,0.965,0.970,		0.975,0.980,0.985,0.990,0.995,1.000
};
void	minChange(float *pnum1,float *pnum2,float *pnum3,float s){
	float	vtTemp=1.0;
	//float *vpTemp=&vtTemp;
	float *vpTemp=&vtTemp;
	if(*vpTemp>*pnum1)
		vpTemp=pnum1;
	if(*vpTemp>*pnum2)
		vpTemp=pnum2;
	if(*vpTemp>*pnum3)
		vpTemp=pnum3;
	if(s>0.8)
		*vpTemp*=1.0/3.0;
	else if (s>0.7)
		*vpTemp*=1.5/3.0;
	else if (s>0.6)
		*vpTemp*=2.0/3.0;
	else if (s>0.5)
		*vpTemp*=2.5/3.0;
	else if (s>0.4)
		*vpTemp*=2.8/3.0;
}

u8 userU8absDec(u8 num1,u8 num2){
	if(num1>=num2)
		return	(num1-num2);
	else
		return	num2-num1;
}

void	 rgbBrightnessAdjust(float *rate,UINT16 h){
	u8 vtS=0;
	float vtAdjRate=0;
	vtS=h%120;
	vtS=userU8absDec(vtS,60);
	vtAdjRate=(RGB_MAX_RATE-RGB_MIN_RATE)/60;
	*rate=RGB_MIN_RATE+vtAdjRate*vtS;
}


void hsb2Rgb(colorStructType * colorStruct) {
	UINT16 	h=colorStruct->hues;
//	float	 s=colorStruct->saturation;
	float s=0;
//	float vtRgbRate=0;
	float	 v=colorStruct->brightness;
	float r = 0, g = 0, b = 0;
	float f,p,q,t,total;
	int i=0;
	s=saturationArray[(u8)(colorStruct->saturation*100)];
	if(h>=360)
		h=0;
	if(s>=1.0)
		s=1.0;
	if(v>=1.0)
		v=1.0;
	i = ( h / 60) % 6;
	f = ((float)h / 60) - i;
	p = v * (1 - s);
	q = v * (1 - f * s);
	t = v * (1 - (1 - f) * s);
	switch (i) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	case 5:
		r = v;
		g = p;
		b = q;
		break;
	default:
		break;
	}

	//minChange(&r,&g,&b,s);

	total=r+g+b;
//	r=r*v/total;
//	g=g*v/total;
//	b=b*v/total;
	r/=total;
	g/=total;
	b/=total;
	if(v>=BrightnessThreshold){
		r*=1/RedGreenRate;
		b*=1/BlueGreenRate;

//		total=r+g+b;
//		r=r*v/total;
//		g=g*v/total;
//		b=b*v/total;
	}else{
		r*=0.90/RedGreenRate+(RedGreenRateStep*(BrightnessThreshold-v)/BrightnessGap);
		b*=1.15/BlueGreenRate+(BlueGreenRateStep*(BrightnessThreshold-v)/BrightnessGap);
//		total=r+g+b;
//		r=r*v/total;
//		g=g*v/total;
//		b=b*v/total;
//		if(r){
//			r*=1-RGBLED_REAL_START_DUTY_PERCENT;
//			r+=RGBLED_REAL_START_DUTY_PERCENT;
//		}
//		if(g){
//			g*=1-RGBLED_REAL_START_DUTY_PERCENT;
//			g+=RGBLED_REAL_START_DUTY_PERCENT;
//		}
//		if(b){
//			b*=1-RGBLED_REAL_START_DUTY_PERCENT;
//			b+=RGBLED_REAL_START_DUTY_PERCENT;
//		}

	}
	total=r+g+b;
	r=r*v/total;
	g=g*v/total;
	b=b*v/total;

	r*=0.95/1.0;
	g*=0.95/1.0;
	b*=0.95/1.0;

	colorStruct->red=(UINT16) (r*PWM_FRQ_CONST);
	colorStruct->blue=(UINT16) (b*PWM_FRQ_CONST);
	colorStruct->green=(UINT16) (g*PWM_FRQ_CONST);
}

//double HLS2RGBvalue(double n1,double n2, double hue)
//{
//    if(hue > 360)
//        hue -= 360;
//    else if(hue < 0)
//        hue += 360;
//    if(hue < 60)
//        return n1+(n2-n1)*hue/60;
//    else if(hue < 180)
//        return n2;
//    else if(hue < 240)
//        return n1+(n2-n1)*(240-hue)/60;
//    else
//        return n1;
//}
//double Hue_2_RGB (double v1, double v2, double vH)
//{
//    if (vH < 0)
//        vH += 1;
//    if (vH > 1)
//        vH -= 1;
//    if ((6*vH) < 1)
//        return (v1 + (v2-v1)*6*vH);
//    if ((2*vH) < 1)
//        return v2;
//    if ((3*vH) < 2)
//        return (v1 + (v2-v1)*((2.0/3.0)-vH)*6);
//
//    return v1;
//}
//
//void hsb2Rgb(colorStructType * colorStruct) {
//	double cmax, cmin;
//	double h = colorStruct->hues/360;
//	double s = colorStruct->saturation;
//	double l = colorStruct->brightness;
//	float r = 0, g = 0, b = 0;
//	if (l <= 0.5)
//		cmax = l * (1 + s);
//	else
//		//cmax = l * (1 - s) + s;
//		cmax = l+s-(l*s);
//	cmin = 2 * l - cmax;
//
//	if (s == 0) {
//		r = g = b = l;
//	} else {
//		r = Hue_2_RGB(cmin, cmax, h +(1.0/3.0));
//		g = Hue_2_RGB(cmin, cmax, h);
//		b = Hue_2_RGB(cmin, cmax, h - (1.0/3.0));
//	}
//	colorStruct->red = (UINT16) (r * PWM_FRQ_CONST );
//	colorStruct->green = (UINT16) (g * PWM_FRQ_CONST );
//	colorStruct->blue = (UINT16) (b * PWM_FRQ_CONST );
//
//}


//  float c=0;
//  float  h2=0;
//  float x =0;
//  float r=0;
//  float g=0;
//  float b=0;
//  float m=0;
//  if(colorStruct->brightness <= 0.5)
//	  c = 2 * colorStruct->brightness * colorStruct->saturation;
//  else c = (2 - 2 * colorStruct->brightness) * colorStruct->saturation;
//  h2= floor(colorStruct->saturation/60);
//  x=c * ( 1 - abs((h2 % 2 )- 1) );
//  switch(h2)
//  {
//      case 0:
//      r = c;
//      g = x;
//      b = 0;
//      break;
//      case 1:
//      r = x;
//      g = c;
//      b = 0;
//      break;
//      case 2:
//      r = 0;
//      g = c;
//      b = x;
//      break;
//      case 3:
//      r = 0;
//      g = x;
//      b = c;
//      break;
//      case 4:
//      r = x;
//      g = 0;
//      b = c;
//      break;
//      case 5:
//      r = c;
//      g = 0;
//      b = x;
//      break;
//      default:
//      g = g = b = 0;
//      break;
//  }
//  m = colorStruct->brightness - 0.5 * c;
//  r += m;
//  g += m;
//  b += m;
////  R = Math.floor( R * 255);
////  G = Math.floor( G * 255);
////  B = Math.floor( B * 255);
////  return R * 65536 + G * 256 + B;
//  		colorStruct->red=(UINT16) (r*PWM_FRQ_CONST*0.68);
//  		colorStruct->green=(UINT16) (g*PWM_FRQ_CONST*0.706);
//  		colorStruct->blue=(UINT16) (b*PWM_FRQ_CONST*0.905);
//}
/*****************************************************************
 *
 * 关闭RGB灯通道
 *
 ******************************************************************/
void turnOffRGBLamp(void){
	user_Pca_CapData16Set(ModuleRed,0);			//Red
	user_Pca_CapData16Set(ModuleGreen,0);			//Green
	user_Pca_CapData16Set(ModuleBlue,0);			//Blue
}

/*****************************************************************
 *
 * 关闭色温灯通道
 *
 ******************************************************************/
void turnOffColorTempLamp(void){
	user_Pca_CapData16Set(ModuleMw,0);				//暖光
	user_Pca_CapData16Set(ModuleCw,0);				//冷光
}

void updateColor(colorStructType * colorStruct){
	turnOffColorTempLamp();
	user_Pca_CapData16Set(ModuleRed,vTemperatureCoe*(colorStruct->red));			//Red
	user_Pca_CapData16Set(ModuleGreen,vTemperatureCoe*(colorStruct->green));		//Green
	user_Pca_CapData16Set(ModuleBlue,vTemperatureCoe*(colorStruct->blue));		//Blue
}

//void updateColorTemp(displayParamsStruct * disParams) {
//	u16 vtCoolData=0, vtWarmData=0;
//	turnOffRGBLamp();
//	vtCoolData = colorTempCoolArray[(disParams->colorTemperature - MIN_ColorTemp)];
//	vtWarmData = PWM_MAX_CW - vtCoolData;
//	user_Pca_CapData16Set(ModuleCw, vTemperatureCoe*vtCoolData * (((float) disParams->brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100));			//从4%起步
//	user_Pca_CapData16Set(ModuleMw, vTemperatureCoe*vtWarmData * (((float) disParams->brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100));			//从4%起步
//}
void updateColorTemp(displayParamsStruct * disParams) {
	u16 vtCoolData=0, vtWarmData=0;
	turnOffRGBLamp();
//	vtCoolData = colorTempCoolArray[(disParams->colorTemperature - MIN_ColorTemp)];
//	vtWarmData = PWM_MAX_CW - vtCoolData;
	vtCoolData = colorTempCoolArray[(disParams->colorTemperature - MIN_ColorTemp)];
//	vtWarmData = PWM_MAX_CW - colorTempWarmArray[(disParams->colorTemperature - MIN_ColorTemp)];
	vtWarmData =colorTempWarmArray[(disParams->colorTemperature - MIN_ColorTemp)];

	if(vtCoolData){
		vtCoolData-=LED_REAL_START_DATA;
		user_Pca_CapData16Set(ModuleCw, vTemperatureCoe*(vtCoolData *(((float) disParams->brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100)+LED_REAL_START_DATA));			//从4%起步
	}else{
		user_Pca_CapData16Set(ModuleCw, 0);
	}
	if(vtWarmData){
		vtWarmData-=LED_REAL_START_DATA;
		user_Pca_CapData16Set(ModuleMw, vTemperatureCoe*(vtWarmData *(((float) disParams->brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100)+LED_REAL_START_DATA));			//从4%起步
	}else{
		user_Pca_CapData16Set(ModuleMw, 0);
	}
}
void	setTargeColorTempData(displayParamsStruct * disParams,PcaDataStruct *targetData){
	u16 vtCoolData=0, vtWarmData=0;
	vtCoolData = colorTempCoolArray[(disParams->colorTemperature - MIN_ColorTemp)];
	vtWarmData = colorTempWarmArray[(disParams->colorTemperature - MIN_ColorTemp)];
	if(vtCoolData){
		vtCoolData-=LED_REAL_START_DATA;
//		targetData->valueOfCw=vtCoolData * (((float) disParams->brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100);
		targetData->valueOfCw=vtCoolData *(((float) disParams->brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100)+LED_REAL_START_DATA;
	}else{
		targetData->valueOfCw=0;

	}
	if(vtWarmData){
		vtWarmData-=LED_REAL_START_DATA;
		//targetData->valueOfMw=vtWarmData * (((float) disParams->brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100);
		targetData->valueOfMw=vtWarmData *(((float) disParams->brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100)+LED_REAL_START_DATA;
	}else{
		targetData->valueOfMw=0;
	}
}


void superFlashMode(u8 index){
	u16 vtCoolData=0;
	vtCoolData = colorTempCoolArray[index];
	user_Pca_CapData16Set(ModuleCw, vtCoolData*vTemperatureCoe);						//从4%起步
	user_Pca_CapData16Set(ModuleMw, 0);												//从4%起步
}

void updateBrightnessByTemperature(void){
	user_Pca_CapData16Set(ModuleRed,vTemperatureCoe*(pcaDataStruct.valueOfRed));			//Red
	user_Pca_CapData16Set(ModuleGreen,vTemperatureCoe*(pcaDataStruct.valueOfGreen));		//Green
	user_Pca_CapData16Set(ModuleBlue,vTemperatureCoe*(pcaDataStruct.valueOfBlue));		//Blue
	user_Pca_CapData16Set(ModuleCw,vTemperatureCoe*(pcaDataStruct.valueOfCw));			//cool
	user_Pca_CapData16Set(ModuleMw,vTemperatureCoe*(pcaDataStruct.valueOfMw));			//warm
}

