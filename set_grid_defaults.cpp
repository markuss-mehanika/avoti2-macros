macro_command main()
  unsigned short a
  a = 4
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  a = 2
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  a = 125
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  a = 250
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
end macro_command