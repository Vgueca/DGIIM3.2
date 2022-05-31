#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "../include/comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
  bool bikini = false;
  bool zapatillas = false;
  bool operator<(const estado &n) const
	{
		if ((fila > n.fila) or (fila == n.fila and columna > n.columna) or
			(fila == n.fila and columna == n.columna and orientacion > n.orientacion) or
			(fila == n.fila and columna == n.columna and orientacion == n.orientacion and zapatillas > n.zapatillas) or
			(fila == n.fila and columna == n.columna and orientacion == n.orientacion and zapatillas == n.zapatillas and bikini > n.bikini))
			return true;
		else
			return false;
	}
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      hayPlan = false;
      ultimaAccion = actIDLE;
      bikini = false;
      zapatillas = false;
      pasamos_objetivo = 0;
      inicio = true;
      fila = -1;
      columna = -1;
      orientacion = -1;
      pintado = false;
      coeficiente = 0.5;
      casillas_totales = mapaResultado.size()*mapaResultado.size();
      casillas_conocidas = 0;
      hayrecarga = false;
      encontrar_recarga = false;
      

    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      hayPlan = false;
      ultimaAccion = actIDLE;
      bikini = false;
      zapatillas = false;
      pasamos_objetivo = 0;
      inicio = true;
      fila = -1;
      columna = -1;
      orientacion = -1;
      pintado = false;
      coeficiente = 0.5;
      casillas_totales = mapaResultado.size()*mapaResultado.size();
      casillas_conocidas = 0;
      hayrecarga = false;
      encontrar_recarga = false;
     
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    estado actual;
    list<estado> objetivos;
    list<Action> plan;
    bool hayPlan;
    int fila;
    int columna;
    int orientacion;
    bool bikini;
    bool zapatillas;
    int pasamos_objetivo;
    Action ultimaAccion;
    bool pintado;
    bool inicio;
    double coeficiente;
    int casillas_totales;
    int casillas_conocidas;
    estado recarga;
    bool hayrecarga;
    bool encontrar_recarga;
    estado anterior;

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan, Sensores sensores);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    
    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);
    bool pathFinding_Aestrella(const estado &origen, const estado &destino, list<Action> &plan);
    int heuristica(const estado &origen, const estado &destino);
    int calcular_costo(estado &origen, Action accion);
    bool crearObjetivos(Sensores sensores);
    int distanciaaEstado(Sensores sensores, estado estado);
    bool llego_borde(Sensores sensores);
    bool objetivosPredefinido(Sensores sensores );
    bool pintar_borde(Sensores sensores);
    bool pintarMapa(Sensores sensores);
    
};

#endif

