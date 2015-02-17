function extractIpf {
	# Create the destination folder
	mkdir ../extract/$1/
	
	# Extract the IPF to the destination folder
	quickbms.exe ./ipf.bms ../../Setup/TreeOfSavior/data/$1 ../extract/$1/
}

# Loop through all the IPFs
for ipfFile in $(ls ../../Setup/TreeOfSavior/data/) ; do
	extractIpf $ipfFile;
done;
