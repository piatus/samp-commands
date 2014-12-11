/*
	samp-commands
	Copyright ( C ) 2014, mrdrifter
	Thx to Gamer_Z, Pamdex, Zeex, SA-MP Team

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	( at your option ) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "plugin.h"

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_PROCESS_TICK | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load( void **ppData ) {
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = ( logprintf_t ) ppData[PLUGIN_DATA_LOGPRINTF];
	
	logprintf(" samp-commands by mrdrifter loaded succesfully!");
	logprintf(" samp-commands version %s", PVERSION);

	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	
}

cell AMX_NATIVE_CALL n_OnPlayerCommandText( AMX* amx, cell* params )
{
	if( !PluginEnabled ) return 1;

	if(params[0] != 8) {
		return logprintf("[samp-commands] bad parametrs on native n_OnPlayerCommandText"),1;
	}
	#define playerid params[1]
	char* cmdtext = NULL;
	int idx;

    amx_StrParam( amx, params[2], cmdtext );

	cell ret=1;

	if( !amx_FindPublic( amx, "OnPlayerCommandReceived", &idx ) )
	{
		amx_PushString( amx, &amx_addr[0], NULL, cmdtext, NULL, NULL );
		amx_Push( amx, static_cast<cell>( playerid ) );
			 
		amx_Exec( amx, &ret, idx );
		amx_Release( amx, amx_addr[0] );
	}	
	if( ret == 0 ) return 1;
	
	ret=0;
	auto it = registerCommands.find( f_MakeCmd( std::string( cmdtext ) ).c_str() );
	if( it != registerCommands.end() )
	{
		int numstr=0, numarg=0;
		ret=1;
		if( it->second->rcIsFormat )
		{	
			
			std::string cmdtxt = string("");
			std::string tmp;

			std::vector<std::string> vect = string_split(cmdtext, ' ');
			if(vect.size() > 1) for(auto sit = vect.rbegin()+1; sit != vect.rend(); ++sit) cmdtxt.insert(cmdtxt.size(), " " + *sit);

			char* format = ( char* )it->second->rcFormat.c_str();
			int itmp=cmdtxt.find_first_of(" ");
			int j = strlen(cmdtxt.c_str())-1; 
			
			if( j > 0 )
				while(j-- > 0) 
					if(cmdtxt[j] == ' ' && cmdtxt[j+1] == ' ') cmdtxt.erase( j, 1 ); 
				else 
					if(cmdtxt[0] == ' ') cmdtxt.erase( 0, 1 ); 
			
			j = strlen( format );
		  
			while( j )
			{
				j--;
				if( cmdtxt.length() <= 1 ) break;
				if( ( unsigned char )*( format + j ) <= ( unsigned char )' ' ) continue;

				if ( *( format + j ) == 'd' )
				{
					itmp = cmdtxt.find_first_of(" ");
					if( itmp >= cmdtxt.length() ) break;

					tmp = cmdtxt.substr ( 0, itmp );  
					cmdtxt.replace ( 0, itmp, "" );  

					amx_Push( it->second->rcAmx, static_cast<cell>( atoi( tmp.c_str() ) ) );
					numarg++;
				}
				else if ( *( format + j ) == 'f' )
				{
					itmp = cmdtxt.find_first_of( " " );
					if( itmp >= cmdtxt.length() ) break;

					tmp = cmdtxt.substr ( 0, itmp );  
					cmdtxt.replace ( 0, itmp, "" );  
					
					float value=( float )atof( tmp.c_str() );
					amx_Push( it->second->rcAmx, amx_ftoc( value ) );
					
					numarg++;
					
				}
				else if ( *( format + j ) == 's' )
				{
					int size = f_GetSizeString( format + j + 1 );
					itmp = cmdtxt.length()-1;
					
					if( size > itmp ) size = itmp;

					tmp = cmdtxt.substr ( 0, size );  
					cmdtxt.replace ( 0, size, "" );  
					 
					amx_PushString( it->second->rcAmx, &amx_addr[numstr], NULL, tmp.c_str(), NULL, NULL );
					
					numstr++;
					numarg++;

				}
			} 
			 
		} else {
			std::string cmdtxt = std::string( cmdtext );
			idx=cmdtxt.find_first_of( " " );

			if( idx >= 1 ) cmdtxt.replace( 0, idx+1, "" );
			else {
				cmdtxt.clear();
				//cmdtxt.insert(0, "");
			}
			amx_PushString( it->second->rcAmx, &amx_addr[numstr], NULL, cmdtxt.c_str(), NULL, NULL );
			numstr++;
		}
		
		amx_Push( it->second->rcAmx, static_cast<cell>( playerid ) );
		if( it->second->rcIsFormat ) amx_Push( it->second->rcAmx, static_cast<cell>( numarg ) );
		 
		amx_Exec( it->second->rcAmx, NULL, it->second->rcIdx );
		while ( numstr ) amx_Release( it->second->rcAmx, amx_addr[( numstr--,numstr )] );

	} else {
		ret=0;
	}
 
	if( !amx_FindPublic( amx, "OnPlayerCommandPerformed", &idx ) )
	{
		amx_Push( amx, static_cast<cell>( ret ) );
		amx_PushString( amx, &amx_addr[0], NULL, cmdtext, NULL, NULL );
		amx_Push( amx, static_cast<cell>( playerid ) );
			 
		amx_Exec( amx, &ret, idx );
		amx_Release( amx, amx_addr[0] );
	}	
		
	
	#undef playerid
	return true;
}
cell AMX_NATIVE_CALL n_PluginEnabled( AMX* amx, cell* params )
{
	if(params[0] != 4) {
		return logprintf("[samp-commands] bad parametrs on native n_PluginEnabled"),1;
	}
	PluginEnabled = params[1] == 1?(1):(0);
	return 1;
}
cell AMX_NATIVE_CALL n_IsPluginEnabled( AMX* amx, cell* params )
{
	return PluginEnabled;
}

cell AMX_NATIVE_CALL n_RegisterCmd( AMX* amx, cell* params )
{
	if(params[0] != 8) {
		return logprintf("[samp-commands] bad parametrs on native commands_RegisterCmd"),1;
	}
    char* cmdName = NULL;
	char* cmdFormat = NULL;
	int idx;

    amx_StrParam( amx, params[1], cmdName );
    amx_StrParam( amx, params[2], cmdFormat );
	
	if( cmdName == NULL || cmdName == 0 ) return 0;
	if( registerCommands.find( string( cmdName ) ) != registerCommands.end() ) return 0;
	if( amx_FindPublic( amx, f_MakeCallback( cmdName ).c_str(), &idx ) ) return 0;

	structRC *rc = new structRC();

	rc->rcFormat = std::string( cmdFormat );
	rc->rcIdx = idx;
	rc->rcAmx = amx;
	  
	if( cmdFormat == NULL || cmdFormat == 0 || !strcmp( cmdFormat, "brak" ) )
		rc->rcFormat = std::string( "brak" ), rc->rcIsFormat = false;
	else 
		rc->rcFormat = std::string( cmdFormat ), rc->rcIsFormat = true;

	registerCommands.insert( std::make_pair( std::string( cmdName ), rc ) );
	
	/*
		registerCommands.emplace( std::pair<std::string,std::pair<std::string,int>>( std::string( cmdName ), std::pair<std::string,int>( std::string( cmdFormat ),idx ) ) );
   */
    return 1;   
}
int f_SearchCommands( AMX* amx ){
	
	logprintf( "[samp-commands] Rozpoczêto ³adowanie komend ( amx %X )", amx );

	AMX_HEADER *hdr = ( AMX_HEADER * )amx->base;
	int numcmd=0;
	for ( int idx = 0,num = ( hdr->natives - hdr->publics )/hdr->defsize; idx < num; ++idx )
		if( !strncmp( reinterpret_cast<char*>( amx->base
                                   + reinterpret_cast<AMX_FUNCSTUBNT*>( hdr->publics
                                            + amx->base )[idx].nameofs ), "cmdr_", 5 ) )
												amx_Exec( amx, NULL, idx ),numcmd++;    
	logprintf( "[samp-commands] Za³adowano %d %s ( amx %X )", numcmd, ( ( numcmd==1 )?( "komende" ):( ( ( numcmd% 10>1 )&&( numcmd% 10<5 )&&!( ( numcmd% 100>=10 )&&( numcmd% 100<=21 ) ) )?( "komendy" ):( "komend" ) ) ), amx );
	return 1;
}
int f_GetSizeString( char * const input )
{
	if ( *input == '[' )
	{
		int length = atoi( input+1 );

		if ( length <= 0 ) length = 0;
		return length;
	}

	return 0;

}
			 
std::vector<std::string> string_split(std::string s, const char delimiter)
{
    size_t start=0;
    size_t end=s.find_first_of(delimiter);
    
    std::vector<std::string> output;
    
    while (end <= std::string::npos)
    {
	    output.emplace_back(s.substr(start, end-start));

	    if (end == std::string::npos)
	    	break;

    	start=end+1;
    	end = s.find_first_of(delimiter, start);
    }
    
    return output;
}
string f_MakeCallback( std::string str2 ){
	
	std::string str=str2;
	std::transform( str.begin(), str.end(), str.begin(), ::tolower );
	str.insert( 0, "cmdf_" );
	return str;
}

string f_MakeCmd( std::string str2 ){
	
	std::string str=str2;
	std::transform( str.begin(), str.end(), str.begin(), ::tolower );
	str.replace( 0,1,"" );
	int fristspace=str.find_first_of( " " );
	if( fristspace >= 1 ) str.replace( fristspace, str.length(), "" );
	return str;
}
 
AMX_NATIVE_INFO PluginNatives[] =
{
	{"commands_RegisterCmd", n_RegisterCmd},
	{"commands_PluginEnabled", n_PluginEnabled},
	{"commands_IsPluginEnabled", n_IsPluginEnabled},
	{"commands_OnPlayerCommandText", n_OnPlayerCommandText},
    {0, 0}
};

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
	if( ProcessTickTime > 0 )
		if( ( ProcessTickTime--, ProcessTickTime ) == 0 )
		{
			for( std::list <AMX *>::iterator i = toExec.begin(); i != toExec.end(); ++i )
				f_SearchCommands( *i ), ProcessTickTime = 0;
			
			toExec.clear();
		}
}
PLUGIN_EXPORT int PLUGIN_CALL AmxLoad( AMX *amx ) 
{
	toExec.push_back( amx );
	allAmx.push_back( amx );
	
	if(!PluginEnabled) return amx_Register( amx, PluginNatives, -1 );
	ProcessTickTime = 100;

	 
    return amx_Register( amx, PluginNatives, -1 );
}

int f_ClearMap( AMX *amx ){
	for( auto outer_iter=registerCommands.begin(); outer_iter!=registerCommands.end(); ++outer_iter ) {
		if( outer_iter->second->rcAmx == amx ) return registerCommands.erase( outer_iter ), f_ClearMap( amx );
    }
	return 1;
}
PLUGIN_EXPORT int PLUGIN_CALL AmxUnload( AMX *amx ) 
{
	toExec.remove( amx );
	allAmx.remove( amx );
	 
	f_ClearMap( amx );

    return AMX_ERR_NONE;
}