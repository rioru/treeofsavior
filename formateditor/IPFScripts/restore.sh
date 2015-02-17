function restoreIpf {
	# Restore the original IPF
	rm "../../Setup/TreeOfSavior/data/$1"
	cp "../../Setup/TreeOfSavior/data/$1.backup" "../../Setup/TreeOfSavior/data/$1"
}

restoreIpf $1