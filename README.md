# nova
A coding/programming language


     MAIN MODULE

* print("Texto")

  Descripción: Imprime una cadena de texto literal en la terminal.

  Ejemplo: print("Hola desde Nova")

* print(variable)

  Descripción: Imprime el valor numérico almacenado en una variable.

  Ejemplo: print(counter)

* nombre = valor

  Descripción: Asigna un valor numérico entero o copia el valor de otra variable.

  Ejemplo: counter = 5 o total = counter

2. Control de Flujo (Motor Principal main.c)

* while condición { ... }

  Descripción: Ejecuta un bloque de código repetidamente mientras se cumpla la condición numérica (actualmente soporta comparaciones como variable > número).

  Ejemplo:

  while counter > 0 {

    print(counter)
    counter = counter - 1

  }

* Comentarios (//)

  Descripción: Ignora cualquier línea que comience con doble barra para dejar notas o documentar el código.

  Ejemplo: // Esto es un comentario