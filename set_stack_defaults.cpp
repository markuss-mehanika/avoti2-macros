macro_command main()
  unsigned short rows, cols, box_width_mm, box_length_mm
  // TODO: might not be set, consider using GetDataEx
  GetData(rows, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  GetData(cols, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  GetData(box_width_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  GetData(box_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  
  unsigned short default_width, default_length, one_by_one = 1, worst_case = 16
  float default_height = 100.0
  default_width = cols*box_width_mm
  default_length = rows*box_length_mm

  SetData(one_by_one, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfLayers")
  SetData(default_height, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxHeight")
  SetData(default_width, "Local HMI", RECIPE, "Avoti_paletesana.PaletteWidth")
  SetData(default_length, "Local HMI", RECIPE, "Avoti_paletesana.PaletteLength")
  SetData(default_height, "Local HMI", RECIPE, "Avoti_paletesana.PaletteHeight")
end macro_command