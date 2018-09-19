/*

*/

#include<stdio.h>
#include<math.h>

//DEFINICOES
#define PI 3.141592653589
//RAIO medio DA TERRA
#define RT 6371.0

//STRUCTS
//struct cord.
typedef struct {
  float dlat;
  float dlon;
} Coordenada;

//FUNCOES
//prototipo de FUNCAO coordFim
Coordenada coordFim(float lat, float lon, int azi, float d);

int main(){

  //arquivo de entrada
  char IN_P[] = "predata_CPQD_man_P.txt";
  char IN_I[] = "predata_CPQD_man_I.txt";

  //nome arq saida
  char OUT_PARAM[] = "saida_param.txt";
  char OUT_P[] = "saida_CPQD_filtro_70.txt";
  char OUT_C_P[] = "saida_CPQD_circ_70.txt";

  char OUT_I[] = "saida_CPQD_filtro_36.txt";
  char OUT_C_I[] = "saida_CPQD_circ_36.txt";

  FILE *in_p, *in_i;
  FILE *out_p, *out_i, *out_c_p, *out_c_i, *out_param;

//abrir entradas
  if( (in_p=fopen(IN_P,"r")) == NULL ) printf("Arquivo nao pode ser aberto\n");
  if( (in_i=fopen(IN_I,"r")) == NULL ) printf("Arquivo nao pode ser aberto\n");
//abrir entrada saida - parametros
  if( (out_param=fopen(OUT_PARAM,"r+a")) == NULL ) printf("Arquivo nao pode ser aberto\n");
//abrir saidas - Protegido e circulo
  if( (out_p=fopen(OUT_P,"w")) == NULL ) printf("Arquivo nao pode ser aberto\n");
  if( (out_c_p=fopen(OUT_C_P,"w")) == NULL ) printf("Arquivo nao pode ser aberto\n");
//abrir saidas - Interferente e circulo
  if( (out_i=fopen(OUT_I,"w")) == NULL ) printf("Arquivo nao pode ser aberto\n");
  if( (out_c_i=fopen(OUT_C_I,"w")) == NULL ) printf("Arquivo nao pode ser aberto\n");

//filtrar campo Protegido
  float dist_maior=0.0;
  float filtro=70.0, campo, lat, lon, dist;
  int azi;

  while( !feof(in_p) ){
    fscanf(in_p,"%f", &lat);
    fscanf(in_p,"%f", &lon);
    fscanf(in_p,"%f", &campo);
    fscanf(in_p,"%f", &dist);
    fscanf(in_p,"%d", &azi);

    if(campo >= filtro){
      fprintf(out_p, "%f %f %f %f %d\n", lat, lon, campo, dist, azi );

      if(dist > dist_maior){
        dist_maior = dist;
      }
    }
  }

  fclose(in_p);
  fclose(out_p);

//atualizar saida param e criar circulo
  char nome[50];
  int canal, i;
  float lon1, lat1;
  Coordenada dcor; //struct para guardar a coord final dada a dist e o azimute

//leitura saida param
  fgets(nome,50,out_param);
  fscanf(out_param,"%d\n",&canal);
  fscanf(out_param,"%f\n",&lon1);
  fscanf(out_param,"%f\n",&lat1);

//criar circulo
  for(i=0; i < 360; i++){

    dcor=coordFim(lat1, lon1, i, dist_maior);

    if(i == 0)  fprintf(out_param, "%f\n", dcor.dlat);  //Norte
    if(i == 90) fprintf(out_param, "%f\n", dcor.dlon);  //Leste
    if(i == 180) fprintf(out_param, "%f\n", dcor.dlat);  //Sul
    if(i == 270) fprintf(out_param, "%f\n", dcor.dlon);  //Oeste

    fprintf(out_c_p, "%f %f %f %f %d\n", dcor.dlon, dcor.dlat, filtro, dist_maior, i );
  }

  //Termina de trabalhar no arquivo out_c_p - saida circulo
      fclose(out_c_p);

//faz o filtro do Interferente
    filtro = 36.0;
    dist_maior = 0.0;

    while( !feof(in_i) ){
      fscanf(in_i,"%f", &lat);
      fscanf(in_i,"%f", &lon);
      fscanf(in_i,"%f", &campo);
      fscanf(in_i,"%f", &dist);
      fscanf(in_i,"%d", &azi);

    if(campo >= filtro){
      fprintf(out_i, "%f %f %f %f %d\n", lat, lon, campo, dist, azi );

      if(dist > dist_maior){
        dist_maior = dist;
      }
    }
  }

  fclose(in_i);
  fclose(out_i);

//atualizar saida param e criar circulo

  for(i=0; i < 360; i++){

    dcor=coordFim(lat1, lon1, i, dist_maior);

    if(i == 0)  fprintf(out_param, "%f\n", dcor.dlat);  //Norte
    if(i == 90) fprintf(out_param, "%f\n", dcor.dlon);  //Leste
    if(i == 180) fprintf(out_param, "%f\n", dcor.dlat);  //Sul
    if(i == 270) fprintf(out_param, "%f\n", dcor.dlon);  //Oeste

  fprintf(out_c_i, "%f %f %f %f %d\n", dcor.dlon, dcor.dlat, filtro, dist_maior, i );
}

//Termina de trabalhar no arquivo out_c_i - saida circulo
    fclose(out_c_i);

  return 0;
}


//FUNCAO calcula a coordenada final
//dado a coordenada inicial, distancia e azimute
Coordenada coordFim(float lat, float lon, int brng, float d){
  Coordenada coord;
  float razang, azimute;

  //para radianos
  lat = lat*PI/180.0;
  lon = lon*PI/180.0;
  azimute = (float) (brng)*PI/180.0;
  razang = d/RT;

  coord.dlat = asin( sin(lat)*cos(razang) + cos(lat)*sin(razang)*cos(azimute));

  coord.dlon = lon + atan2(sin(azimute)*sin(razang)*cos(lat),
               cos(razang)-sin(lat)*sin(coord.dlat));

  coord.dlat = coord.dlat*180.0/PI;
  coord.dlon = coord.dlon*180.0/PI;

  return coord;
}
