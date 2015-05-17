make

for chosen_m in 1000 10000 100000 200000 300000 400000
do
	echo "Running on chosen m of $chosen_m"
	echo "RUNNING MASTER"
	./Test_DistributedHE_HELib_master $chosen_m &> output/test_machine/out_master_$chosen_m.txt
	cat output/test_machine/out_master_$chosen_m.txt
	echo "RUNNING MODIFIED"
	for num_nodes in 4 8 16
	do
		echo "Running modified on $num_nodes nodes"
		mpirun --mca btl_tcp_if_include eth0 --hostfile hostfile -n $num_nodes ./Test_DistributedHE_HELib_modified $chosen_m &> output/test_machine/out_modified_$chosen_m.$num_nodes.txt
		cat output/test_machine/out_modified_$chosen_m.$num_nodes.txt
	done
done
