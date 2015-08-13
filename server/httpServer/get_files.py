import requests

host = "http://d28848zpt823xm.cloudfront.net"
headers = {'user-agent': 'tos'}

files = [
    "toscdn/patch/updater/patcher.version.txt", 
    "toscdn/patch/full/data.revision.txt",
    "toscdn/patch/full/release.revision.txt",
    "toscdn/patch/full/data.file.list.txt",
    "toscdn/patch/partial/pre.revision.txt",
    "toscdn/patch/partial/data.revision.txt",
    "toscdn/patch/partial/release.revision.txt",
];

for file in files:
    r = requests.get (host + "/" + file, headers=headers);
    f = open (file, "wb+");
    f.write (r.text);
    f.close ();