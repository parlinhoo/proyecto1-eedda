# Backwards Non-deterministic DAWG Matching

## Definición

Backwards Non-Deterministic DAWG Matching (BNDM) es un algoritmo creado por Gonzalo Navarro y Mathieu Raffinot. Consiste en un híbrido de técnicas creadas anteriormente para el problema de encontrar las coincidencias de un patrón dentro de un texto, de las cuales son principalmente autómatas finitos no-deterministas y BDM.

El uso de autómatas finitos no-deterministas puede parecer a simple vista algo que existe solo en la teoría, pero con los últimos métodos inventados, se puede simular el paralelismo de éstos, logrando cubrir muchas opciones simultáneamente, como se mostrará más adelante.

BDM (Backwards DAWG Matching), por otro lado, consiste en usar un autómata representado por un grafo llamado DAWG (Grafo Acíclico Dirigido de Palabras, por sus siglas en inglés), el cual acepta cualquier entrada que corresponda a un sufijo de una palabra en particular. Después de ser creado, se itera por el texto comparando los sufijos de cada ventana con el patrón, para establecer cuántos espacios se pueden saltar a la derecha.

BNDM} combina esos dos conceptos, junto operaciones a nivel de bits para efectivamente simular los distintos caminos de un autómata no determinista, y así poder emular un paso de éste con una sola instrucción de procesador.

## Implementación

La implementación para encontrar un solo patrón es relativamente sencilla. Solo hay que generar un vector de bit por cada letra del alfabeto $c \in \Sigma$ del largo del patrón el cual tome valor 1 en cada posición en la que esté presente en él.

Se define $D$ como un vector de bits con solo 1s, luego se empieza con una ventana del tamaño del patrón y se comienza a iterar desde el final hacia el inicio aplicándole la máscara $B[c]$ a $D$ por cada caracter por donde pasa el puntero. Si en algún punto $D$ se vuelve 0 (es decir, todos sus bits se vuelven 0), ya no puede haber una coincidencia, así que continúa al siguiente paso. En cambio, si ya revisó toda la ventana y $D$ no es 0, entonces hay una coincidencia y guarda su posición como $pos+1$

Finalmente, $D$ se le aplica un bitshift a la izquierda y se actualiza la posición de la ventana según un valor $last$, que se obtiene al guardar la última posición en donde el último bit de $D$ era 1.

Para claridad, a continuación se presenta una demostración paso a paso del algoritmo según el artículo.
![Algoritmo aplicado](https://github.com/parlinhoo/proyecto1-eedda/blob/main/algoritmo.png)


Lo fundamental para relacionar éste algoritmo con un autómata finito no-determinista es darse cuenta que $D$ representa los estados de cada una de las posiciones de los caracteres de la ventana, y aplicar la máscara es hacer un paso del autómata, que vuelve 0 todos los "estados" que no tienen una transición en el grafo.

Este algoritmo se puede utilizar no solo para buscar un solo patrón, si no que puede ser implementado con múltiples patrones, en donde se utiliza un símbolo "comodín" para representar símbolos que pueden tomar cualquier valor, en el caso que hayan varios patrones con distinto tamaño.
Los únicos cambios con la implementación de un solo patrón son que el bitshift ahora mueve $D$ según la cantidad de patrones a la izquierda, y que ahora el patrón toma la forma $p^1_1p^2_1\ldots p^k_1p^1_2p^2_2\ldots p^k_m$, alternando entre los caracteres de los patrones.

Para revisar en detalle la implementación, se tiene que conocer una estructura extra $bitvector$ que tiene la siguiente forma:
```cpp
class bitvector {
private:
    static const size_t word_size = 8*sizeof(unsigned long);
    size_t size_longs;
    size_t size_bits;
    unsigned long* vector;

public:
    size_t get_size() const;
    std::string get_bit_string() const;
    void set_bit(size_t bit, bool value);
    bool get_bit(size_t bit) const;
    bool is_zero() const;
    
    bitvector operator|(bitvector& vec2) const;
    bitvector operator&(bitvector& vec2) const;
    bitvector operator^(bitvector& vec2) const;
    bitvector operator~() const;
    bitvector operator<<(size_t bits) const;
    bitvector operator>>(size_t bits) const;

    bitvector& operator&=(bitvector& vec2);
    bitvector& operator<<=(size_t vec2);

    bitvector(size_t size_bits);
    bitvector(size_t size_bits, bool zeros);
    bitvector(const bitvector& vec2);
    ~bitvector();
};
```

Este vector de bits ayuda a soportar cualquier largo de patrones, ya que define un arreglo de $longs$ que guardan todos los bits, dependiendo de la cantidad de bits requerida. Éste código se puede encontrar en [éste repositorio](https://github.com/parlinhoo/proyecto1-eedda/tree/main).

Luego, la primera fase consiste en el preprocesamiento, que implica agregar los símbolos comodines a las palabras y construir el patrón principal. Este estracto del código muestra esa parte.

```cpp
	std::string main_pattern("");

	std::map<char, size_t> index;
    std::vector<bitvector> vectors;
    bool wildcard = false;

    size_t max_size = 0;

    for (uint i = 0; i < patterns.size(); i++) {
        if (patterns[i].length() > max_size) {
            max_size = patterns[i].length();
        }
    }

    for (uint i = 0; i < max_size; i++) {
        for (std::string pat : patterns) {
            if (i < (max_size - pat.length())) {
                main_pattern.push_back(WILDCARD);
                wildcard = true;
                continue;
            }
            main_pattern.push_back(pat.at(i - max_size + pat.length()));
        }
    }

    std::cout << main_pattern << std::endl;

    for (uint i = 0; i < main_pattern.length(); i++) {
        char char_i = main_pattern.at(i);

        auto pair = index.try_emplace(char_i, vectors.size());
        if (pair.second) {
            vectors.push_back(bitvector(main_pattern.length()));
        }

        vectors.at(index[char_i]).set_bit(main_pattern.length()-1-i, 1);
    }

    if (wildcard) {
        for (int i = 0; i < vectors[index[WILDCARD]].get_size(); i++) {
            if (vectors[index[WILDCARD]].get_bit(i)) {
                for (int j = 0; j < vectors.size(); j++) {
                    vectors[j].set_bit(i, 1);
                }
            }
        }
    }
```

Finalmente, el algoritmo es relativamente corto ya que simplemente tiene que iterar por todo el texto, solamente cambiando de ventana, como sigue:

```cpp
	while (pos <= (text.length() - max_size)) {
        int j = max_size;
        int last = max_size;
        bitvector D(main_pattern.length(), false);
        while (!D.is_zero()) {
            D &= vectors[index[text.at(pos + j - 1)]];
            j--;
            for (size_t i = 0; i < patterns.size(); i++) {
                if (D.get_bit(main_pattern.length()-1-i)) {
                    if (j > 0) last = j;
                    else occurences[i].push_back(pos + max_size - patterns[i].length());
                }
            }
            D <<= patterns.size();
        }
        pos += last;
    }
```

Así, se obtendrá un vector de vectores que contienen las ocurrencias de cada uno de los patrones buscados en el mismo orden de entrada.

## Aplicaciones
Este algoritmo tiene muchas aplicaciones, ya que la búsqueda rápida de patrones en un texto es algo que se hace muy comúnmente. Además, este algoritmo supera en velocidad a muchos algoritmos conocidos como KMP, la familia de algoritmos Boyer-Moore y muchos otros más.

## Referencias
Navarro, G., & Raffinot, M. (1998, July). A bit-parallel approach to suffix automata: Fast extended string matching. In _Annual Symposium on Combinatorial Pattern Matching_ (pp. 14-33). Berlin, Heidelberg: Springer Berlin Heidelberg. 
