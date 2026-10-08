@tool
extends SceneTree

func _init() -> void:
	var f = FileAccess.open("d:/testing/Adaptive Procedural Level Generation/test_log.txt", FileAccess.WRITE)
	f.store_line("--- INSPECT START ---")
	
	var main_scene = load("res://Scenes/Main.tscn").instantiate()
	root.add_child(main_scene)
	
	f.store_line("Main scene instantiated!")
	var tilemap: TileMapLayer = main_scene.get_node_or_null("TileMapLayer")
	if tilemap:
		f.store_line("TileMapLayer exists. Used cells count: " + str(tilemap.get_used_cells().size()))
		for cell in tilemap.get_used_cells():
			var src_id = tilemap.get_cell_source_id(cell)
			var atlas_coord = tilemap.get_cell_atlas_coords(cell)
			f.store_line("Cell " + str(cell) + " src_id=" + str(src_id) + " atlas_coord=" + str(atlas_coord))
			
	f.store_line("--- Tree Structure ---")
	_write_tree(root, 0, f)
	f.store_line("--- INSPECT END ---")
	f.close()
	quit()

func _write_tree(n: Node, depth: int, f: FileAccess) -> void:
	var prefix = "  ".repeat(depth)
	f.store_line(prefix + n.name + " (" + n.get_class() + ")")
	for c in n.get_children():
		_write_tree(c, depth + 1, f)
