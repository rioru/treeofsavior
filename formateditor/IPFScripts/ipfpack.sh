
function packIpf {
	# Create the backup
	mkdir -p "../backup/$1/"
	cp "../../Setup/TreeOfSavior/data/$1" "../backup/$1/$1.backup"
	
	# Restore the original IPF
	rm "../../Setup/TreeOfSavior/data/$1"
	cp "../../Setup/TreeOfSavior/data/$1.backup" "../../Setup/TreeOfSavior/data/$1"
	
	# Clean the repack folder
	rm -rf "../repack";
	
	for fileExtract in $2 ; do
		# Create a new repack folder
		dirIpf=$(dirname $fileExtract);
		mkdir -p "../repack/$1/$dirIpf"
		
		# Put the target file in the repack folder
		cp "../extract/$1/$fileExtract" "../repack/$1/$fileExtract"
	done
	
	# Repack with QuickBMS
	echo "Repacking $2...";
	quickbms.exe -w -r "./ipf.bms" "../../Setup/TreeOfSavior/data/$1" "../repack/$1"
}

if [ "$#" -ne 2 ]; then
    echo "Usage : ipfpack.sh <ipfName.ipf> <extracted file [other files]>"
	echo "Exemple : ipfpack.sh addon.ipf quickslotnexpbar/quickslotnexpbar.lua"
	exit 0;
fi

packIpf "$1" "$2";
