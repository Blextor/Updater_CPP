Code Blocks MinGW-t használtam, így annak a beállításait írom le.
Projektnek a Build Setting-ben kell módosítani az "Other linker options"-t a Linker settings-ben, és a Search directories-ban a Compliler és a Linker fül alatt kell a mappákat hozzáadni.
Fontos, hogy a Dev mappát könnyen elérhető helyen kell tartani. Lehetőleg speciálisabb karaktereket nem tartalmazó mappákban.

Ezt követően fordulnia kéne a projekteknek. 
Az .exe futtatásához a .dll fájlok nem szükségesek, ha statikus linkelést használuk Build-eléshez, különben kellhetnek.
