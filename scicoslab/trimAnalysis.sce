scf(1); clf(1);
data = read('data/f16.log',-1,1);
plot(log10(1:size(data,1)),log10(data));
xlabel('log10(iteration)');
ylabel('log10(cost)');
title('f16 - level, 500 ft/s');
legend('newton raphson')
xs2eps(1,'f16-level-500fts')

//scf(2); clf(2);
//data = read('data/c172p.log',-1,1);
//plot(log10(1:size(data,1)),log10(data));
//xlabel('log10(iteration)');
//ylabel('log10(cost)');
//title('c172p - level, 20 ft/s');
//legend('newton raphson')
//xs2eps(2,'c172p-level-20fts')
