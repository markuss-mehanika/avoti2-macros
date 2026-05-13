macro_command main()
  unsigned short one_by_one[4] = {1,1,125,250}, worst_case[4] = {5,2,125,250}
  SetData(one_by_one[0], "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  SetData(one_by_one[1], "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  SetData(one_by_one[2], "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  SetData(one_by_one[3], "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
end macro_command