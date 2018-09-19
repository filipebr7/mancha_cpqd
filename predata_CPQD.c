/**
Software:   CPQd_aux
Autor:      Filipe Ribeiro
Data:       31/01/2018

Descrição:
Este programa faz o tratamento de dados da saída do software CPQd.
Trata os dados do arquivo (.man) e tem como saída um arquivo (.txt) com:

LONGITUDE (decimal), LATITUDE (decimal), CAMPO (dBu V/m), DISTANCIA (km),
 AZIMUTE (graus)

Como parametros de saída (saida_param.txt) para o arquivo kml temos:
                i) LONG e LAT da torre
                  (obtida da leitura do cabeçalho do arquivo do CPQd)
                ii)  Canal e decalagem
                  (obtido da leitura do cabeçalho do arquivo do CPQd)
                iii) Norte, Sul, Leste, Oeste
                  (quando distancia é 100km) (calculado pelo programa)

Última Modificação:     02/02/2018
*/

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<math.h>
#include<string.h>

//DEFINICOES
#define PI 3.141592653589
//RAIO medio DA TERRA
#define RT 6371.0
//TAM 1 = Protegido  TAM 2 = Interferente
#define TAM 2

//STRUCTS
//struct cord.
typedef struct {
  float dlat;
  float dlon;
} Coordenada;

//FUNCOES
//converte DMS para decimal
float convDMS(int DMS);
//prototipo de FUNCAO coordFim
Coordenada coordFim(float lat, float lon, int azi, float d);

//COMECA AQUI
int main(int argc, char* argv[]){
    if (argc == 1){
      printf("Programa sem parametros.\n");
      return 1;
    }

    //ARQS SAIDA
    char OUT[] = "predata_CPQD_man_I.txt";
    //char IN_campo[100];
    char OUT_param[] = "saida_param.txt";

    /** Cabeçalho arquivo CPQd */
    char ID[50];
    int  cnl, dec;   //lat2, long2

    FILE *in_c, *out, *out2;

    if( (in_c=fopen(argv[1],"r")) == NULL ) printf("Arquivo nao pode ser aberto\n");
    if( (out=fopen(OUT,"w")) == NULL ) printf("Arquivo nao pode ser aberto\n");
    if( (out2=fopen(OUT_param,"w")) == NULL ) printf("Arquivo nao pode ser aberto\n");

    int i, azi = -1, linha = -1;
    float campo, long0, lat0, long1, lat1;
    float conv = PI/180.0;
    char c;

    int long2, lat2;
    float dist;

    Coordenada dcor;

/** Leitura cabeçalho do arquivo CPQd*/
//linha 1 - ID string - Nome do local
    fgets(ID,50,in_c);
    fprintf(out2, "%s",ID);
//linha 2 - Canal (int)
    fscanf(in_c,"%d\n",&cnl);
//linha 3 - Decalagem int
    fscanf(in_c,"%d\n",&dec);
    if (dec == -1){
        fprintf(out2,"%d-\n",cnl);
    } else if (dec == 1){
        fprintf(out2,"%d+\n",cnl);
    }else{
        fprintf(out2,"%d\n",cnl);
    }

//linha 4 - Latitude int
    fscanf(in_c,"%d\n",&lat2);
//linha 5 - Longitude int
    fscanf(in_c,"%d\n",&long2);

/** Transforma a LAT e LONG para decimal e escreve nos parametros*/
            lat0 = convDMS(lat2);
            long0 = convDMS(long2);
    //printf("LONG: %f \t LAT: %f\n", long0, lat0);

            fprintf(out2,"%f\n", long0);
            fprintf(out2,"%f\n", lat0);

/** Trabalhando no arquivo */
    while ( !feof(in_c) ){
        c = fgetc(in_c);
        //printf( "%c" , c ) ;

        if ( c == '#' ){
            azi++; //em graus
            while ( linha < TAM*400 ){
                linha++;
                dist=linha*0.25; //distancia em km

                //printf("%c", fgetc(in));
                fscanf(in_c,"%f\n", &campo);
                if( dist == 0 ) fscanf(in_c,"%f\n", &campo);

                //calcula a coordenada final
                dcor=coordFim(lat0, long0, azi, dist);
                                //escreve a saida
                fprintf(out,"%f %f %f %f %d\n", dcor.dlon, dcor.dlat, campo, dist , azi);
            }//loop distancia
            linha = -1; //zera linha
        }//loop azimute
    }//loop arquivo

//Termina de trabalhar no arquivo
    fclose(in_c);
    fclose(out);
    fclose(out2);

    return 0;
}

//FUNCAO converte DMS PARA decimal
float convDMS(int DMS){
  int aux_s, aux_m, aux_g;
  float seg, min, graus;

  aux_s = DMS%100;
  seg = (float)aux_s/3600.0;
  aux_m = DMS/100;
  aux_m = aux_m%100;
  min = (float)aux_m/60.0;
  aux_g = DMS/10000;
  graus = (float) seg+min+aux_g;

  return graus;
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
