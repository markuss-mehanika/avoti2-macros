macro_command main()
  unsigned short a
  float b
  a = 1
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfLayers")
  a = 4
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  a = 2
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  a = 125
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  a = 250
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  b = 100.0
  SetData(b, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxHeight")
  a = 350
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.PaletteWidth")
  a = 1100
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.PaletteLength")
  b = 500
  SetData(b, "Local HMI", RECIPE, "Avoti_paletesana.PaletteHeight")
end macro_command