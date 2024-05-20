all:
	g++ -o create_db_table.out create_db_table.cpp `mysql_config --cflags --libs`
	g++ -o select_table_hk_address.out select_table_hk_address.cpp `mysql_config --cflags --libs`
clean:
	rm *.out