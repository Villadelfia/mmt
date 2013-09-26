tga_encode, Randy Thiemann 2013
-------------------------------

Dit programma is getest op OS X en Windows en geeft op beide systemen de output
files die te vinden zijn in de bin map (.tga zijn de originelen, .rle.tga zijn 
encoded).

Op Windows geeft cmake een werkende VS2012 solution file. Op OS X maakt u best
een map build aan, en in die map typt u dan "cmake .. && make". Dit geeft een
werkende executable.

In de map ConfigReader zit mijn ConfigReader klasse, getest met alle line-ending
types, deze accept en reject configurations zoals aangegeven in de specificatie.
