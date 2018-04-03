function best_ave(name)
  global besttimes;
  global avetimes;
  bestname=strcat(name, '.bestindivtimes');
  besttimes=dlmread(bestname,'\n');
  avename=strcat(name, '.avetimes');
  avetimes=dlmread(avename,'\n');
  plot( linspace(1,length(besttimes),length(besttimes)), besttimes, 'k.-',              linspace(1,length(avetimes), length(avetimes)),  avetimes,  'k.:' );
  title( strrep(name, '_', ' ') );
  ylabel( 'time' );
  xlabel( 'generation' );
  legend( 'best indiv time', 'pop average time' );
