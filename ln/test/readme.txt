Truecrypt-Passwort: lntest

lntest_fileid_src.tc enth�lt das Laufwerk, das als Source fungiert. Es enth�lt eine Junction zu P:\Testjunction

lntest_fileid_junctions.tc enth�lt das Verzeichnis Testjunction, dessen File-IDs sich mit Dir1 in lntest_fileid_src.tc �berschneiden. Wenn lntest_fileid_junctions.tc als P: gemountet wird, funktioniert die Junction aus lntest_fileid_src.tc.

Wenn nun die gemountete lntest_fileid_src.tc mit --copy --unroll kopiert wird, werden ca. 50% der Dateien wegen ID-�berlappung falsch verlinkt.