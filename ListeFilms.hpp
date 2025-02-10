#pragma once

#include "structures.hpp"
#include <span>
#include <string>

class ListeFilms {
public:
	ListeFilms(const string& nomFichier);
	void ajouterFilm(Film* filmPtr);
	void enleverFilm(Film* filmAEnleverPtr);
	std::span<Film*> spanListeFilms() const;

	friend std::span<Acteur*> spanListeActeur(ListeActeurs& listeActeurs);
	friend Acteur* chercherActeur(const ListeFilms& listeFilms, const string& nomActeur);


private:
	int capacite_, nElements_;
	Film** elements_; // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.
};