<div align="center">

<h1>Inserción vs. Quicksort con mediana de tres</h1>

<p><b>Comparación empírica de dos algoritmos de ordenación en C++</b></p>

<p>
Universidad Nacional Jorge Basadre Grohmann<br>
Escuela Profesional de Ingeniería en Informática y Sistemas<br>
Programación Avanzada | Sección B | Semestre 2026-II
</p>

<p>
<img src="https://img.shields.io/badge/lenguaje-C%2B%2B17-00599C?style=flat-square" alt="C++17">
<img src="https://img.shields.io/badge/estado-en%20desarrollo-orange?style=flat-square" alt="Estado">
</p>

</div>

<hr>

<h2>Descripción</h2>

<p>
Este proyecto compara el ordenamiento por inserción, un método directo, con Quicksort, un método de divide y vencerás, midiendo su comportamiento sobre distintos tamaños y patrones de entrada. El énfasis está en el diseño descriptivo y en la reproducibilidad del estudio.
</p>

<h2>Objetivos</h2>

<ul>
  <li>Implementar el ordenamiento por inserción y Quicksort con pivote mediana de tres.</li>
  <li>Medir tiempo de ejecución, comparaciones, intercambios y profundidad de recursión.</li>
  <li>Calcular estadística descriptiva (media, mediana, desviación estándar, mínimo y máximo) y relacionarla con la complejidad teórica.</li>
  <li>Documentar el estudio de forma que pueda reproducirse.</li>
</ul>

<h2>Algoritmos comparados</h2>

<table>
  <thead>
    <tr>
      <th align="left">Característica</th>
      <th align="left">Inserción</th>
      <th align="left">Quicksort (mediana de tres)</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>Mejor caso</td><td>O(n)</td><td>O(n log n)</td></tr>
    <tr><td>Caso promedio</td><td>O(n²)</td><td>O(n log n)</td></tr>
    <tr><td>Peor caso</td><td>O(n²)</td><td>O(n²)</td></tr>
    <tr><td>Memoria auxiliar</td><td>O(1)</td><td>O(log n) de pila en promedio</td></tr>
    <tr><td>Estabilidad</td><td>Estable</td><td>Inestable</td></tr>
    <tr><td>Recursión</td><td>No</td><td>Sí</td></tr>
  </tbody>
</table>

<p>
Quicksort usa el esquema de partición de Hoare y elige como pivote la mediana entre el primer, el central y el último elemento.
</p>

<h2>Diseño experimental</h2>

<table>
  <tbody>
    <tr><td><b>Tamaños (n)</b></td><td>1 000, 10 000 y 100 000</td></tr>
    <tr><td><b>Patrones</b></td><td>aleatorio, ascendente, descendente, casi ordenado (5 % permutado) y con duplicados</td></tr>
    <tr><td><b>Repeticiones</b></td><td>30 por condición (algoritmo, patrón, tamaño)</td></tr>
    <tr><td><b>Semilla</b></td><td>fija, registrada en cada fila de resultados</td></tr>
    <tr><td><b>Reloj</b></td><td><code>std::chrono::steady_clock</code>, sin incluir generación de datos ni escritura en disco</td></tr>
  </tbody>
</table>

<p>Cada ejecución se registrará en un archivo CSV con las columnas:</p>

<pre><code>algoritmo, n, patron, repeticion, tiempo_ms, comparaciones, intercambios, profundidad_recursion, semilla, commit_hash</code></pre>

<p>
Las instrucciones de compilación, ejecución y regeneración de resultados, junto con el entorno de pruebas (hardware, sistema operativo, compilador, semilla y commit final), se agregarán a este archivo al terminar la implementación.
</p>

<h2>Referencias principales</h2>

<ul>
  <li>Cormen, T. H., Leiserson, C. E., Rivest, R. L., &amp; Stein, C. (2022). <i>Introduction to algorithms</i> (4.ª ed.). MIT Press.</li>
  <li>Hoare, C. A. R. (1962). Quicksort. <i>The Computer Journal, 5</i>(1), 10-16. <a href="https://doi.org/10.1093/comjnl/5.1.10">https://doi.org/10.1093/comjnl/5.1.10</a></li>
  <li>Knuth, D. E. (1998). <i>The art of computer programming: Vol. 3. Sorting and searching</i> (2.ª ed.). Addison-Wesley.</li>
  <li>Sedgewick, R. (1978). Implementing Quicksort programs. <i>Communications of the ACM, 21</i>(10), 847-857.</li>
</ul>
