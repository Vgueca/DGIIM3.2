#include "../Comportamientos_Jugador/jugador.hpp"
#include "../include/motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
#include <random>
#include <algorithm>

int ComportamientoJugador::distanciaaEstado(Sensores sensores, estado estado){
	int distancia = 0;
	distancia = (abs(sensores.posF- estado.fila)+abs(sensores.posC-estado.columna));
	return distancia;
}

bool ComportamientoJugador::objetivosPredefinido(Sensores sensores){
	int fila_1, columna_1;
	int fila_c, columna_c;
	fila_c = columna_c = mapaResultado.size()/2;
	estado central;
	central.fila = fila_c;
	central.columna = columna_c;

	random_device rd;
    default_random_engine eng(rd());
	uniform_int_distribution<int> distr(4, mapaResultado.size()-4);

	int aux = 4 + rand()%(mapaResultado.size()-7);
	int distancia[4];

	estado state;
	state.fila = aux;
	state.columna = 4;
	distancia[0] = distanciaaEstado(sensores , state);

	estado state2;
	state.fila = 4;
	state.columna = aux;
	distancia[1] = distanciaaEstado(sensores , state2);

	estado state3;
	state.fila = mapaResultado.size()-4;
	state.columna = aux;
	distancia[2] = distanciaaEstado(sensores , state3);

	estado state4;
	state.fila = mapaResultado.size()-4;
	state.columna = aux;
	distancia[3] =distanciaaEstado(sensores , state4);

	int minima = distancia[0];
	int indice = -1;
	for(int i = 0; i < 3; i++){
		if(minima >= distancia[i]){
			minima = distancia[i];
			indice = i;
		}
	}
	switch(indice){
		case 0:
			objetivos.push_back(state);
		break;
		case 1:
			objetivos.push_back(state2);
		break;
		case 2:
			objetivos.push_back(state3);
		break;
		case 3:
			objetivos.push_back(state4);
		break;
	}

	objetivos.push_back(central);
	
}
//Funcion que crea dos objetivos desconocidos actualmente para que mediante el algoritmo A estrella nuestro jugador los persiga.
bool ComportamientoJugador::crearObjetivos(Sensores sensores){
	int contador = 0;
	objetivos.clear();
	estado objetivo1, objetivo2;
	int fila1, columna1;
	double distancia_objetivo1 = 0, distancia_objetivo2 = 0;
	int distancia_maxima = 20;
	int filaMax, columnaMax, filaMin, columnaMin;

	//CÃ³digo para poder generar un random que se encuentre dentro del mapa (quitando precipicios)
	random_device rd;
    default_random_engine eng(rd());
	uniform_int_distribution<int> distr(4, mapaResultado.size()-4);

	filaMax = min(sensores.posF + distancia_maxima, (int)mapaResultado.size()-4);
	columnaMax = min(sensores.posC + distancia_maxima, (int)mapaResultado.size()-4);
	filaMin = max(sensores.posF - distancia_maxima, 4);
	columnaMin = max(sensores.posC - distancia_maxima, 4);
	fila1 = filaMin+rand()%(filaMax+1-filaMin);
	columna1 = columnaMin+rand()%(columnaMax+1-columnaMin);
	objetivo1.fila = fila1;
	objetivo1.columna = columna1;
	int distancia = distanciaaEstado(sensores, objetivo1);
	while((mapaResultado[fila1][columna1] != '?'  && contador < 50) or (fila1 == sensores.posF && columna1 == sensores.posC) /*or(distancia > distancia_maxima)*/ ){
		filaMax = min(sensores.posF + distancia_maxima, (int)mapaResultado.size()-4);
		columnaMax = min(sensores.posC + distancia_maxima, (int)mapaResultado.size()-4);
		filaMin = max(sensores.posF - distancia_maxima, 4);
		columnaMin = max(sensores.posC - distancia_maxima, 4);
		fila1 = filaMin+rand()%(filaMax+1-filaMin);
		columna1 = columnaMin+rand()%(columnaMax+1-columnaMin);
		objetivo1.fila = fila1;
		objetivo1.columna = columna1;
		distancia = distanciaaEstado(sensores, objetivo1);
		contador++;
	}
	contador = 0;

	//Creamos los dos objetivos (los demÃ¡s parÃ¡metros de estos objetivos no nos interesan)
	objetivo1.fila = fila1;
	objetivo1.columna = columna1;
	objetivo2.fila = columna1;
	objetivo2.columna = fila1;

	distancia_objetivo1 = distanciaaEstado(sensores, objetivo1);
	distancia_objetivo2 = distanciaaEstado(sensores, objetivo2);

	if(distancia_objetivo1 > distancia_objetivo2){
		objetivos.push_back(objetivo2);
		objetivos.push_back(objetivo1);
	}else{
		objetivos.push_back(objetivo1);
		objetivos.push_back(objetivo2);
	}


	return true;
}

bool ComportamientoJugador::pintar_borde(Sensores sensores){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < mapaResultado.size(); j++){
			mapaResultado[i][j] = 'P';
			mapaResultado[j][i] = 'P';
		}
	}
	for(int i = mapaResultado.size()-3 ; i < mapaResultado.size(); i++){
		for(int j = 0; j < mapaResultado.size(); j++){
			mapaResultado[i][j] = 'P';
			mapaResultado[j][i] = 'P';
		}
	}
	return true;
}
bool ComportamientoJugador::llego_borde(Sensores sensores){
	if(fila == 4 or fila == mapaResultado.size()-4){
		return true;
	}else if(columna == 4 or columna == mapaResultado.size()-4){
		return true;
	}else{
		return false;
	}
}

bool ComportamientoJugador::pintarMapa(Sensores sensores){
	int contador = 0;
	int fil, col;
	if(sensores.nivel == 3){
		fil = sensores.posF;
		col = sensores.posC;
		orientacion = sensores.sentido;
	}else{
		fil = fila;
		col = columna;
		
	}
	bool hay_recalcular = false;
	int bosque_sinZapas= 0;
	int agua_sinBikini = 0;
	bool precipicio = false;
	bool muro = false;
	bool estaEnPlan;

	

	switch (orientacion)
	{
	case 0: // Orientacion Norte
		for(int i=0; i<4; i++){
			for(int j=-i; j<=i; j++){
				if(mapaResultado[fil-i][col+j] == '?'){
					mapaResultado[fil-i][col+j] = sensores.terreno[contador];
					casillas_conocidas++;
					if(mapaConPlan[fil-i][col+j] == 1){
						if(mapaResultado[fil-i][col+j] == 'A' && bikini == false){
							agua_sinBikini++;
						}else if(mapaResultado[fil-i][col+j] == 'B' && zapatillas == false){
							bosque_sinZapas++;
						}
						else if(mapaResultado[fil-i][col+j] == 'M' ){
							muro = true;
						}
						else if(mapaResultado[fil-i][col+j] == 'P'){
							precipicio = true;
						}
					}
					if(mapaResultado[fil-i][col+j]== 'X'){
						recarga.fila= fil-i;
						recarga.columna = col+j;
						hayrecarga = true;
					}
				}
				contador++;
			}
		}
		break;
	
	case 2: // Orientacion Este
		for(int i=0; i<4; i++){
			for(int j=-i; j<=i; j++){
				if(mapaResultado[fil+j][col+i] == '?'){
					mapaResultado[fil+j][col+i] = sensores.terreno[contador];
					casillas_conocidas++;
					if(mapaConPlan[fil+j][col+i] == 1){
						if(mapaResultado[fil+j][col+i] == 'A' && bikini == false){
							agua_sinBikini++;
						}else if(mapaResultado[fil+j][col+i] == 'B' && zapatillas == false){
							bosque_sinZapas++;
						}
						else if(mapaResultado[fil+j][col+i] == 'M' ){
							muro = true;
						}
						else if(mapaResultado[fil+j][col+i] == 'P'){
							precipicio = true;
						}
					}
					if(mapaResultado[fil+j][col+i]== 'X'){
						recarga.fila= fil+j;
						recarga.columna = col+i;
						hayrecarga = true;
					}
				}
				contador++;
			}
		}
		break;

	case 4: // Orientacion Sur
		for(int i=0; i<4; i++){
			for(int j=-i; j<=i; j++){
				if(mapaResultado[fil+i][col-j] == '?'){
					mapaResultado[fil+i][col-j] = sensores.terreno[contador];
					casillas_conocidas++;
					if(mapaConPlan[fil+i][col-j] == 1){
						if(mapaResultado[fil+i][col-j] == 'A' && bikini == false){
							agua_sinBikini++;
						}else if(mapaResultado[fil+i][col-j] == 'B' && zapatillas == false){
							bosque_sinZapas++;
						}
						else if(mapaResultado[fil+i][col-j] == 'M' ){
							muro = true;
						}
						else if(mapaResultado[fil+i][col-j] == 'P'){
							precipicio = true;
						}
					}
					if(mapaResultado[fil+i][col-j]== 'X'){
						recarga.fila= fil+i;
						recarga.columna = col-j;
						hayrecarga = true;
					}
				}
				contador++;
			}
		}
		break;

	case 6: // Orientacion Oeste
		for(int i=0; i<4; i++){
			for(int j=-i; j<=i; j++){
				if(mapaResultado[fil-j][col-i] == '?'){
					mapaResultado[fil-j][col-i] = sensores.terreno[contador];
					casillas_conocidas++;
					if(mapaConPlan[fil-j][col-i] == 1){
						if(mapaResultado[fil-j][col-i] == 'A' && bikini == false){
							agua_sinBikini++;
						}else if(mapaResultado[fil-j][col-i] == 'B' && zapatillas == false){
							bosque_sinZapas++;
						}
						else if(mapaResultado[fil-j][col-i] == 'M' ){
							muro = true;
						}
						else if(mapaResultado[fil-j][col-i] == 'P'){
							precipicio = true;
						}
					}
					if(mapaResultado[fil-j][col-i]== 'X'){
						recarga.fila= fil-j;
						recarga.columna = col-i;
						hayrecarga = true;
					}
				}
				contador++;
			}
		}
		break;
	

	case 1: // Orientacion NorEste		
		for(int i=0; i<4; i++){
			for(int j=0; j<2*i+1; j++){
				if(j<=i){
					if(mapaResultado[fil-i][col+j] == '?'){
						mapaResultado[fil-i][col+j] = sensores.terreno[contador];
						casillas_conocidas++;
						if(mapaConPlan[fil-i][col+j] == 1){
							if(mapaResultado[fil-i][col+j] == 'A' && bikini == false){
								agua_sinBikini++;
							}else if(mapaResultado[fil-i][col+j] == 'B' && zapatillas == false){
								bosque_sinZapas++;
							}
							else if(mapaResultado[fil-i][col+j] == 'M' ){
								muro = true;
							}
							else if(mapaResultado[fil-i][col+j] == 'P'){
								precipicio = true;
							}
						}
						if(mapaResultado[fil-i][col+j]== 'X'){
							recarga.fila= fil-i;
							recarga.columna = col+j;
							hayrecarga = true;
						}
					}
					contador++;
				}
				else{
					if(mapaResultado[fil-i+j-i][col+i] == '?'){
						mapaResultado[fil-i+j-i][col+i] = sensores.terreno[contador];
						casillas_conocidas++;
						if(mapaConPlan[fil-i+j-i][col+i] == 1){
							if(mapaResultado[fil-i+j-i][col+i] == 'A' && bikini == false){
								agua_sinBikini++;
							}else if(mapaResultado[fil-i+j-i][col+i] == 'B' && zapatillas == false){
								bosque_sinZapas++;
							}
							else if(mapaResultado[fil-i+j-i][col+i] == 'M' ){
								muro = true;
							}
							else if(mapaResultado[fil-i+j-i][col+i] == 'P'){
								precipicio = true;
							}
						}
						if(mapaResultado[fil-i+j-i][col+i]== 'X'){
							recarga.fila= fil-i+j-i;
							recarga.columna = col+i;
							hayrecarga = true;
						}
					}
					contador++;
				}
			}
		}
		break;
	
	case 3: // Orientacion SurEste	
		for(int i=0; i<4; i++){
			for(int j=0; j<2*i+1; j++){
				if(j<=i){
					if(mapaResultado[fil+j][col+i] == '?'){
						mapaResultado[fil+j][col+i] = sensores.terreno[contador];
						casillas_conocidas++;
						if(mapaConPlan[fil+j][col+i] == 1){
							if(mapaResultado[fil+j][col+i] == 'A' && bikini == false){
								agua_sinBikini++;
							}else if(mapaResultado[fil+j][col+i] == 'B' && zapatillas == false){
								bosque_sinZapas++;
							}
							else if(mapaResultado[fil+j][col+i] == 'M' ){
								muro = true;
							}
							else if(mapaResultado[fil+j][col+i] == 'P'){
								precipicio = true;
							}
						}
						if(mapaResultado[fil+j][col+i]== 'X'){
							recarga.fila= fil+j;
							recarga.columna = col+i;
							hayrecarga = true;
						}
					}
					contador++;
				}
				else{
					if(mapaResultado[fil+i][col+i-j+i] == '?'){
						mapaResultado[fil+i][col+i-j+i] = sensores.terreno[contador];
						casillas_conocidas++;
						if(mapaConPlan[fil+i][col+i-j+i] == 1){
							if(mapaResultado[fil+i][col+i-j+i] == 'A' && bikini == false){
								agua_sinBikini++;
							}else if(mapaResultado[fil+i][col+i-j+i] == 'B' && zapatillas == false){
								bosque_sinZapas++;
							}
							else if(mapaResultado[fil+i][col+i-j+i] == 'M' ){
								muro = true;
							}
							else if(mapaResultado[fil+i][col+i-j+i] == 'P'){
								precipicio = true;
							}
						}
						if(mapaResultado[fil+i][col+i-j+i]== 'X'){
							recarga.fila= fil+i;
							recarga.columna = col+i-j+i;
							hayrecarga = true;
						}
					}
					contador++;
				}
			}
		}
		break;

	case 5: // Orientacion SurOeste	
		for(int i=0; i<4; i++){
			for(int j=0; j<2*i+1; j++){
				if(j<=i){
					if(mapaResultado[fil+i][col-j] == '?'){
						mapaResultado[fil+i][col-j] = sensores.terreno[contador];
						casillas_conocidas++;
						if(mapaConPlan[fil+i][col-j] == 1){
							if(mapaResultado[fil+i][col-j] == 'A' && bikini == false){
								agua_sinBikini++;
							}else if(mapaResultado[fil+i][col-j] == 'B' && zapatillas == false){
								bosque_sinZapas++;
							}
							else if(mapaResultado[fil+i][col-j] == 'M' ){
								muro = true;
							}
							else if(mapaResultado[fil+i][col-j] == 'P'){
								precipicio = true;
							}
						}
						if(mapaResultado[fil+i][col-j]== 'X'){
							recarga.fila= fil+i;
							recarga.columna = col-j;
							hayrecarga = true;
						}
					}
					contador++;
				}
				else{
					if(mapaResultado[fil+i-j+i][col-i] == '?'){
						mapaResultado[fil+i-j+i][col-i] = sensores.terreno[contador];
						casillas_conocidas++;
						if(mapaConPlan[fil+i-j+i][col-i] == 1){
							if(mapaResultado[fil+i-j+i][col-i] == 'A' && bikini == false){
								agua_sinBikini++;
							}else if(mapaResultado[fil+i-j+i][col-i] == 'B' && zapatillas == false){
								bosque_sinZapas++;
							}
							else if(mapaResultado[fil+i-j+i][col-i] == 'M' ){
								muro = true;
							}
							else if(mapaResultado[fil+i-j+i][col-i] == 'P'){
								precipicio = true;
							}
						}
						if(mapaResultado[fil+i-j+i][col-i]== 'X'){
							recarga.fila= fil+i-j+i;
							recarga.columna = col-i;
							hayrecarga = true;
						}
					}
					contador++;
				}
			}
		}
		break;
	
	case 7: // Orientacion NorOeste	
		for(int i=0; i<4; i++){
			for(int j=0; j<2*i+1; j++){
				if(j<=i){
					if(mapaResultado[fil-j][col-i] == '?'){
						mapaResultado[fil-j][col-i] = sensores.terreno[contador];
						casillas_conocidas++;
						if(mapaConPlan[fil-j][col-i] == 1){
							if(mapaResultado[fil-j][col-i] == 'A' && bikini == false){
								agua_sinBikini++;
							}else if(mapaResultado[fil-j][col-i] == 'B' && zapatillas == false){
								bosque_sinZapas++;
							}
							else if(mapaResultado[fil-j][col-i] == 'M' ){
								muro = true;
							}
							else if(mapaResultado[fil-j][col-i] == 'P'){
								precipicio = true;
							}
						}
						if(mapaResultado[fil-j][col-i]== 'X'){
							recarga.fila= fil-j;
							recarga.columna = col-i;
							hayrecarga = true;
						}
					}
					contador++;
				}
				else{
					if(mapaResultado[fil-i][col-i+j-i] == '?'){
						mapaResultado[fil-i][col-i+j-i] = sensores.terreno[contador];
						casillas_conocidas++;
						if(mapaConPlan[fil-i][col-i+j-i] == 1){
							if(mapaResultado[fil-i][col-i+j-i] == 'A' && bikini == false){
								agua_sinBikini++;
							}else if(mapaResultado[fil-i][col-i+j-i] == 'B' && zapatillas == false){
								bosque_sinZapas++;
							}
							else if(mapaResultado[fil-i][col-i+j-i] == 'M' ){
								muro = true;
							}
							else if(mapaResultado[fil-i][col-i+j-i] == 'P'){
								precipicio = true;
							}
						}
						if(mapaResultado[fil-i][col-i+j-i]== 'X'){
							recarga.fila= fil-i;
							recarga.columna = col-i+j-i;
							hayrecarga = true;
						}
					}
					contador++;
				}
			}
		}
		break;
	}

	if( muro || precipicio || bosque_sinZapas >= 1 || agua_sinBikini >= 1){
		pasamos_objetivo++;
		hay_recalcular = true;
	}else{
		hay_recalcular = false;
	}	
	return hay_recalcular;
}

// Este es el mÃ©todo principal que se piden en la practica.
// Tiene como entrada la informaciÃ³n de los sensores y devuelve la acciÃ³n a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action accion = actIDLE;
	bool recalcular = false;
	double calculo;
	calculo = casillas_conocidas/casillas_totales;
	if(calculo > 0.15 ){
		coeficiente = 0.3;
	}else if(calculo > 0.2){
		coeficiente = 0.2;
	}

	if(sensores.nivel != 4){
		fila = actual.fila = sensores.posF;
		columna = actual.columna = sensores.posC;
		orientacion = actual.orientacion = sensores.sentido;
		cout << "Fila: " << actual.fila << endl;
		cout << "Col : " << actual.columna << endl;
		cout << "Ori : " << actual.orientacion << endl;
	}else{
		cout << "Fila: " << fila << endl;
		cout << "Col : " << columna << endl;
		cout << "Ori : " << orientacion << endl;
		actual.fila = fila;
		actual.columna = columna;
		actual.orientacion = orientacion;
	}
	
	if(sensores.terreno[0] == 'K'){
		bikini = true;
		zapatillas =false;
	}else if(sensores.terreno[0] == 'D'){
		bikini = false;
		zapatillas = true;
	}

	// Capturo los destinos
	cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
	if(sensores.nivel != 3  && sensores.nivel != 4){
		objetivos.clear();
	}
	for (int i = 0; i < sensores.num_destinos; i++)
	{
		estado aux;
		aux.fila = sensores.destino[2 * i];
		aux.columna = sensores.destino[2 * i + 1];
		objetivos.push_back(aux);
	}

	if(plan.empty()){
		hayPlan = false;
	}

	//Estoy en una casilla de recarga y me quedo recargando hasta tener al menos 1000 de batería
	if(sensores.terreno[0] == 'X' && sensores.bateria < 1500){
      	encontrar_recarga = false;
		accion = actIDLE;
		ultimaAccion = accion;
		return accion;
	}


	
	//Si no he colisionado actualizo las variables de posición y la brújula
	if(sensores.colision == false){
		if(sensores.nivel == 3 or sensores.nivel == 4){
			switch (ultimaAccion){
				case actFORWARD:
					switch(orientacion){
						case 0: fila--; break;
						case 1: fila--; columna++ ; break;
						case 2: columna++; break; 
						case 3: fila++; columna++;break;
						case 4: fila++; break;
						case 5: fila++; columna--; break;
						case 6: columna--; break;
						case 7: fila--; columna--; break;
					}
				break;
				case actTURN_L:
					orientacion = (orientacion+6)%8;
				break;
				case actTURN_R:
					orientacion = (orientacion+2)%8;
				break;
				case actSEMITURN_R:
					orientacion = (orientacion+1)%8;
				break;
				case actSEMITURN_L:
					orientacion = (orientacion+7)%8;
				break;
				case actIDLE:
				break;
				case actWHEREIS:
					actual.fila = fila = sensores.posF+2;
					actual.columna = columna = sensores.posC+2;
					actual.orientacion = orientacion = sensores.sentido;
				break;
			}
			recalcular = pintarMapa(sensores);
			if(llego_borde(sensores) && !pintado){
				pintar_borde(sensores);
				pintado = true;
			}
		}
	}
	if(pasamos_objetivo >15 && mapaResultado[objetivos.front().fila][objetivos.front().columna] != 'X'){	//Si hemos recalculado más de 25 veces limpiamos todo y cambiamos de objetivo. 
		pasamos_objetivo = 0;
		hayPlan = false;
		objetivos.clear();
		plan.clear();
	}
	if(sensores.bateria<500 && hayrecarga && !encontrar_recarga){
		if(sensores.nivel == 4 or sensores.nivel == 3){
			encontrar_recarga = true;
			objetivos.clear();
			plan.clear();
			objetivos.push_back(recarga);
			hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan,sensores);
		}
	}
	if(!hayPlan && !inicio){
		if(sensores.nivel != 3){	
			hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan,sensores);
		}else if(sensores.nivel == 3){
			pasamos_objetivo = 0;
			crearObjetivos(sensores);
			hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan,sensores);
		}
	}
	if(!plan.empty() && hayPlan){
		if(sensores.nivel == 3){
			if(sensores.colision == true && sensores.terreno[2] == 'M' ){	//Estamos en el nivel 3 y hemos colisionado contra un muro en nuestro plan intuido a ciegas
				hayPlan = false;
				plan.clear();	//Borramos el plan actual
				objetivos.clear();
				accion = actIDLE;
			}else{
				accion = plan.front();
				plan.pop_front();
			
				if(recalcular){ 
					plan.clear();	

					if(mapaResultado[objetivos.front().fila][objetivos.front().columna] != '?' && mapaResultado[objetivos.front().fila][objetivos.front().columna] != 'X' ){
						
						if(!objetivos.empty())
							objetivos.pop_front(); 
						if(objetivos.empty()){
							crearObjetivos(sensores);
						}
					}

					hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan,sensores);
					
					if(ultimaAccion == actIDLE){
						accion = plan.front();
						plan.pop_front();
					}else{
						accion = actIDLE;
					}
					
					recalcular = false;
				}

			}

		}else if(sensores.nivel == 4){
			if(ultimaAccion == actIDLE){
				accion = actWHEREIS;
			}
			if(sensores.colision == true){
				accion = actIDLE;
				ultimaAccion = accion;
				plan.clear();
				hayPlan = false;
				return accion;
			}else{
				accion = plan.front();
				plan.pop_front();
				if(recalcular){
					
					plan.clear();
					hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan,sensores);
					if(ultimaAccion == actIDLE){
						accion = plan.front();
						plan.pop_front();
					}else{	
						accion = actIDLE;
					}
					recalcular = false;
				}
			}
		}else{
			accion = plan.front();
			plan.pop_front();
		}
		
	}
	if(inicio){
		if(sensores.nivel == 4){
			accion = actWHEREIS;	//La primera accion del nivel 4 será para situarse en el mapa correctamente
		}else{
			accion = actIDLE;
		}
		inicio = false;
	}
	if(sensores.terreno[2] == 'P' && accion == actFORWARD && sensores.nivel > 2){
		hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan,sensores);
		while(!hayPlan){
			crearObjetivos(sensores);
			hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan,sensores);
		}
		
		accion = plan.front();
		plan.pop_front();
	}
	
	ultimaAccion = accion;
	return accion;

}



// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan, Sensores sensores)
{
	estado un_objetivo;
	switch (level)
	{
	case 0:
		cout << "Demo\n";
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Profundidad(origen, un_objetivo, plan);
		break;

	case 1:
		cout << "Optimo numero de acciones\n";
		// Incluir aqui la llamada al busqueda en anchura
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Anchura(origen, un_objetivo, plan);
		break;

	case 2:
		cout << "Optimo en coste\n";
		// Incluir aqui la llamada al busqueda de costo uniforme/A*
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Aestrella(origen, un_objetivo, plan);
		break;
	case 3:
		cout << "Reto 1: Descubrir el mapa\n";
		un_objetivo = objetivos.front();	
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Aestrella(origen, un_objetivo, plan);
		break;
	case 4:
		cout << "Reto 2: Maximizar objetivos\n";
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Aestrella(origen, un_objetivo, plan);
		break;
	}
	return false;
}

//---------------------- ImplementaciÃ³n de la busqueda en profundidad ---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla)
{
	if (casilla == 'P' or casilla == 'M')
		return true;
	else
		return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posiciÃ³n de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st)
{
	int fil = st.fila, col = st.columna;

	// calculo cual es la casilla de delante del agente
	switch (st.orientacion)
	{
	case 0:
		fil--;
		break;
	case 1:
		fil--;
		col++;
		break;
	case 2:
		col++;
		break;
	case 3:
		fil++;
		col++;
		break;
	case 4:
		fil++;
		break;
	case 5:
		fil++;
		col--;
		break;
	case 6:
		col--;
		break;
	case 7:
		fil--;
		col--;
		break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil < 0 or fil >= mapaResultado.size())
		return true;
	if (col < 0 or col >= mapaResultado[0].size())
		return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col]))
	{
		// No hay obstaculo, actualizo el parametro st poniendo la casilla de delante.
		st.fila = fil;
		st.columna = col;
		return false;
	}
	else
	{
		return true;
	}
}

struct nodo
{
	estado st;
	list<Action> secuencia;
};

struct ComparaEstados
{
	bool operator()(const estado &a, const estado &n) const
	{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.zapatillas > n.zapatillas) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.zapatillas == n.zapatillas and a.bikini > n.bikini))
			return true;
		else
			return false;
	}
};

// ImplementaciÃ³n de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	stack<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	queue<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		while(Cerrados.find(Abiertos.front().st) != Cerrados.end()){
			Abiertos.pop();
		}
		
		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.front();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}


/*HeurÃ­stica a seguir: distancia mÃ­nima entre dos estados concretos
*/
int ComportamientoJugador::heuristica(const estado &origen, const estado &destino ){
	return  max(abs(origen.fila-destino.fila),abs(origen.columna-destino.columna));
}


/*Tipo de nodo con los atributos esenciales para poder realizar el Algoritmo A* */
struct nodo_estrella{
	nodo a;
	double g=0;		//coste
	double h;		//heuristica
	double f;

	bool operator<(const nodo_estrella &aux) const{
		return (f) > (aux.f);
	}

	bool operator>(const nodo_estrella &aux) const{
		return (f) < (aux.f);
	}
};

/*Struct que nos permite la comparaciÃ³n de nodos estrella*/
struct ComparaNodos
{
	bool operator()(const nodo_estrella &n1, const nodo_estrella &n2) const
	{
		if (n1.a.st < n2.a.st)
			return true;
		else
			return false;
	}
};


/*FunciÃ³n para calcular el costo de la acciÃ³n llevada a cabo desde un estado concreo*/
int ComportamientoJugador::calcular_costo(estado &origen, Action accion){
	int fila = origen.fila;
	int columna = origen.columna;
	int costo = 0;

	if(mapaResultado[fila][columna] == 'K' && !bikini){
		origen.bikini = true;
		origen.zapatillas = false;
	}else if(mapaResultado[fila][columna] == 'D' && !zapatillas){
		origen.zapatillas = true;
		origen.bikini = false;
	}

	if(mapaResultado[fila][columna] == 'A'){
		if(accion == actFORWARD){
			if(origen.bikini){
				costo = 10;
			}else{
				costo = 200;
			}
		}else if(accion == actTURN_R or accion == actTURN_L ){
			if(origen.bikini){
				costo = 5;
			}else{
				costo = 500;
			}
		}else if(accion == actSEMITURN_R or accion == actSEMITURN_L ){
			if(origen.bikini){
				costo = 2;
			}else{
				costo = 300;
			}
		}else if(accion == actWHEREIS){
			costo = 200;
		}else{
			costo = 0;
		}
	}else if(mapaResultado[fila][columna] == 'B'){
		if(accion == actFORWARD){
			if(origen.zapatillas){
				costo = 15;
			}else{
				costo = 100;
			}
		}else if(accion == actTURN_R or accion == actTURN_L ){
			if(origen.zapatillas){
				costo = 1;
			}else{
				costo = 3;
			}
		}else if(accion == actSEMITURN_R or accion == actSEMITURN_L ){
			if(origen.zapatillas){
				costo = 1;
			}else{
				costo = 2;
			}
		}else if(accion == actWHEREIS){
			costo = 200;
		}else{
			costo = 0;
		}
	}else if(mapaResultado[fila][columna] == 'T'){
		if(accion == actFORWARD){
			costo = 2;
		}else if(accion == actTURN_R or accion == actTURN_L ){
			costo = 2;
		}else if(accion == actSEMITURN_R or accion == actSEMITURN_L ){
			costo = 1;
		}else if(accion == actWHEREIS){
			costo = 200;
		}else{
			costo = 0;
		}
	}else{
		if(mapaResultado[fila][columna] == '?'){				//Estimacion media de los costos de casillas desconocidas en funcion de la accion
			if(accion == actFORWARD){
				costo = 42*coeficiente;
			}else if(accion == actTURN_R or accion == actTURN_L ){
				costo = 65*coeficiente;
			}else if(accion == actSEMITURN_R or accion == actSEMITURN_L ){
				costo = 39*coeficiente;
			}else if(accion == actWHEREIS){
				costo = 200*coeficiente;
			}else{
				costo = 0;
			}
		}else{
			if(accion == actFORWARD){
			costo = 1;
			}else if(accion == actTURN_R or accion == actTURN_L ){
				costo = 1;
			}else if(accion == actSEMITURN_R or accion == actSEMITURN_L ){
				costo = 1;
			}else if(accion == actWHEREIS){
				costo = 200;
			}else{
				costo = 0;
			}
		}
		
		
	}

	return costo;
}


/*Algoritmo Aestrella */
bool ComportamientoJugador::pathFinding_Aestrella(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();

	set<nodo_estrella, ComparaNodos> Cerrados; // Lista de Cerrados
	priority_queue<nodo_estrella> Abiertos;				  // Lista de Abiertos

	nodo_estrella current;
	current.a.st = origen;
	//current.a.secuencia.empty();
	current.g = 0;
	current.h = heuristica(current.a.st, destino);
	current.f = current.g + current.h;

	Abiertos.push(current);
	if(current.a.st.fila == destino.fila && current.a.st.columna == destino.columna){	//En caso de que el objetivo seleccionado sea el estado en el que nos encontramos
		if(!Abiertos.empty()){
			Abiertos.pop();
		}
		current.a.secuencia.push_back(actIDLE);
		Cerrados.insert(current);
	}
	while (!Abiertos.empty() and (current.a.st.fila != destino.fila or current.a.st.columna != destino.columna))
	{ 
		
		Abiertos.pop();
		Cerrados.insert(current);
		//set<nodo_estrella, ComparaNodos>::iterator it;

		// Generar descendiente de girar a la derecha 90 grados
		nodo_estrella hijoTurnR = current;
		hijoTurnR.a.st.orientacion = (hijoTurnR.a.st.orientacion + 2) % 8;
		hijoTurnR.a.secuencia.push_back(actTURN_R);
		hijoTurnR.g += calcular_costo(current.a.st, actTURN_R);
		hijoTurnR.h = heuristica(hijoTurnR.a.st , destino);
		hijoTurnR.f = hijoTurnR.g + hijoTurnR.h;
		//it = Cerrados.find(hijoTurnR);
		if ( Cerrados.find(hijoTurnR) == Cerrados.end())
		{
			Abiertos.push(hijoTurnR);
		}else if(hijoTurnR > *Cerrados.find(hijoTurnR)){
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo_estrella hijoSEMITurnR = current;
		hijoSEMITurnR.a.st.orientacion = (hijoSEMITurnR.a.st.orientacion + 1) % 8;
		hijoSEMITurnR.a.secuencia.push_back(actSEMITURN_R);
		hijoSEMITurnR.g += calcular_costo(current.a.st, actSEMITURN_R);
		hijoSEMITurnR.h = heuristica(hijoSEMITurnR.a.st , destino);
		hijoSEMITurnR.f = hijoSEMITurnR.g + hijoSEMITurnR.h;

		//it = Cerrados.find(hijoSEMITurnR);
		if ( Cerrados.find(hijoSEMITurnR) == Cerrados.end())
		{
			
			Abiertos.push(hijoSEMITurnR);
		}else if(hijoSEMITurnR > *Cerrados.find(hijoSEMITurnR)){
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo_estrella hijoTurnL = current;
		hijoTurnL.a.st.orientacion = (hijoTurnL.a.st.orientacion + 6) % 8;	
		hijoTurnL.a.secuencia.push_back(actTURN_L);
		hijoTurnL.g += calcular_costo(current.a.st, actTURN_L);
		hijoTurnL.h = heuristica(hijoTurnL.a.st , destino);
		hijoTurnL.f = hijoTurnL.g + hijoTurnL.h;

		//it = Cerrados.find(hijoTurnL);
		if ( Cerrados.find(hijoTurnL) == Cerrados.end())
		{
			Abiertos.push(hijoTurnL);
		}else if(hijoTurnL > *Cerrados.find(hijoTurnL)){
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo_estrella hijoSEMITurnL = current;
		hijoSEMITurnL.a.st.orientacion = (hijoSEMITurnL.a.st.orientacion + 7) % 8;
		hijoSEMITurnL.a.secuencia.push_back(actSEMITURN_L);
		hijoSEMITurnL.g += calcular_costo(current.a.st, actSEMITURN_L);
		hijoSEMITurnL.h = heuristica(hijoSEMITurnL.a.st , destino);
		hijoSEMITurnL.f = hijoSEMITurnL.g + hijoSEMITurnL.h;

		//it = Cerrados.find(hijoSEMITurnL);
		if ( Cerrados.find(hijoSEMITurnL) == Cerrados.end())
		{
			Abiertos.push(hijoSEMITurnL);
		}else if(hijoSEMITurnL > *Cerrados.find(hijoSEMITurnL)){
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo_estrella hijoForward = current;
		hijoForward.a.secuencia.push_back(actFORWARD);
		hijoForward.g += calcular_costo(current.a.st, actFORWARD);
		hijoForward.h = heuristica(hijoForward.a.st , destino);
		hijoForward.f = hijoForward.g + hijoForward.h;
		if (!HayObstaculoDelante(hijoForward.a.st))
		{
			//it = Cerrados.find(hijoForward);
			if ( Cerrados.find(hijoForward) == Cerrados.end())
			{
				Abiertos.push(hijoForward);
			}else if(hijoForward > *Cerrados.find(hijoForward)){
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
			while(!Abiertos.empty() && Cerrados.find(current) != Cerrados.end()){
				Abiertos.pop();
				if(!Abiertos.empty()){
					current = Abiertos.top();
				}
			}
		}
	}
	
	cout << "Terminada la busqueda\n";

	if (current.a.st.fila == destino.fila and current.a.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.a.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}


// Sacar por la consola la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan)
{
	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			cout << "A ";
		}
		else if (*it == actTURN_R)
		{
			cout << "D ";
		}
		else if (*it == actSEMITURN_R)
		{
			cout << "d ";
		}
		else if (*it == actTURN_L)
		{
			cout << "I ";
		}
		else if (*it == actSEMITURN_L)
		{
			cout << "i ";
		}
		else
		{
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

// Funcion auxiliar para poner a 0 todas las casillas de una matriz
void AnularMatriz(vector<vector<unsigned char>> &m)
{
	for (int i = 0; i < m[0].size(); i++)
	{
		for (int j = 0; j < m.size(); j++)
		{
			m[i][j] = 0;
		}
	}
}

// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			switch (cst.orientacion)
			{
			case 0:
				cst.fila--;
				break;
			case 1:
				cst.fila--;
				cst.columna++;
				break;
			case 2:
				cst.columna++;
				break;
			case 3:
				cst.fila++;
				cst.columna++;
				break;
			case 4:
				cst.fila++;
				break;
			case 5:
				cst.fila++;
				cst.columna--;
				break;
			case 6:
				cst.columna--;
				break;
			case 7:
				cst.fila--;
				cst.columna--;
				break;
			}
			mapaConPlan[cst.fila][cst.columna] = 1;
		}
		else if (*it == actTURN_R)
		{
			cst.orientacion = (cst.orientacion + 2) % 8;
		}
		else if (*it == actSEMITURN_R)
		{
			cst.orientacion = (cst.orientacion + 1) % 8;
		}
		else if (*it == actTURN_L)
		{
			cst.orientacion = (cst.orientacion + 6) % 8;
		}
		else if (*it == actSEMITURN_L)
		{
			cst.orientacion = (cst.orientacion + 7) % 8;
		}
		it++;
	}
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
