#ifndef ROBOT_H
#define ROBOT_H
typedef struct robotStateBITS {
union {
struct {
unsigned char taskEnCours;
float vitesseGaucheConsigne;
float vitesseGaucheCommandeCourante;
float vitesseDroiteConsigne;
float vitesseDroiteCommandeCourante;
float distanceTelemetreDroit1;
float distanceTelemetreDroit;
float distanceTelemetreCentre;
float distanceTelemetreGauche1;
float distanceTelemetreGauche;
float vitesseDroitFromOdometry;
float vitesseGaucheFromOdometry;
float vitesseLineaireFromOdometry;
float vitesseAngulaireFromOdometry;
float xPosFromOdometry_1;
float xPosFromOdometry;
float yPosFromOdometry_1;
float yPosFromOdometry;
float angleRadianFromOdometry_1;
float angleRadianFromOdometry;

};
};
} ROBOT_STATE_BITS;
extern volatile ROBOT_STATE_BITS robotState;

#endif /* ROBOT_H */
