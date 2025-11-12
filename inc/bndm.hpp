#pragma once

#include <string>
#include <vector>

// Devuelve un vector de vectores con los índices en donde se encontraron los patrones solicitados, en el mismo orden de entrada.
std::vector<std::vector<int>> bndm(std::string text, std::vector<std::string> patterns);
