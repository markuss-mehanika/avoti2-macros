macro_command main()
  unsigned short a = 1
  float b
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfLayers")
  a = 2
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  a = 2
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  a = 125
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  a = 250
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  b = 100.0
  SetData(b, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxHeight")
  a = 250
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.PaletteWidth")
  a = 500
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.PaletteLength")
  b = 50.0
  SetData(b, "Local HMI", RECIPE, "Avoti_paletesana.PaletteHeight")
end macro_command