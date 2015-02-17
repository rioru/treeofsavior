function extractIpf {
	# Create the destination folder
	mkdir ../extract/$1/
	
	# Extract the IPF to the destination folder
	quickbms.exe -Y ./ipf.bms ../../Setup/TreeOfSavior/data/$1 ../extract/$1/
}

extractIpf $1