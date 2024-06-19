
rm -f	moreward.h
touch	moreward.h

printf "#ifndef _MOREWARD_H_\n"				    >>  moreward.h
printf "#define _MOREWARD_H_\n"				    >>  moreward.h

printf "\n/*\n"								    >>  moreward.h
printf "\nLicense info is at the end.\n\n"		>>	moreward.h
cat guide.md							    	>>  moreward.h
printf "\n*/\n\n"							  	>>  moreward.h

sed '1,3d;$d' src/a.h	    					>>  moreward.h
sed '1,3d;$d' src/b.h		    				>>  moreward.h
sed '1,3d;$d' src/c.h			    			>>  moreward.h
sed '1,3d;$d' src/m.h				    		>>  moreward.h

printf "\n\n/* IMPLEMENTATION */\n\n"	    	>>  moreward.h
printf "\n#ifdef MOREWARD_H_IMPL\n\n"	    	>>  moreward.h

sed '1d' src/m_alloc.c						    >>  moreward.h

printf "\n#endif /* MOREWARD_H_IMPL */\n\n"	    >>  moreward.h

printf "/*\n\n"								    >>  moreward.h
cat license.txt							        >>  moreward.h
printf "\n*/\n\n"								>>  moreward.h
printf "#endif /* _MOREWARD_H_ */\n"			>>  moreward.h
