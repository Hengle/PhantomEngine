 # v e r s i o n   4 1 0   c o r e 
 
 l a y o u t   ( l o c a t i o n   =   0 )   o u t   v e c 4   o u t p u t C o l o r ; 
 
 
 l a y o u t (   s t d 1 4 0 )   u n i f o r m   L i g h t 
 { 
         v e c 4   l i g h t P o s ; 
 	 v e c 4   l i g h t C o l o r ; 
 	 v e c 4   l i g h t D i r ; 
 	 m a t 4   l i g h t V P ; 
 }   l i g h t ; 
 
 u n i f o r m   s a m p l e r 2 D   d i f f u s e C o l o r ; 
 u n i f o r m   s a m p l e r 2 D   s h a d o w M a p ; 
 
 i n   v e c 4   v V i e w P o s ; 
 i n   v e c 3   n o r m a l ; 
 i n   v e c 2   t e x c ; 
 i n   v e c 4   v W o r l d P o s ; 
 i n   v e c 4   F r a g P o s L i g h t S p a c e ; 
 
 
 f l o a t   S h a d o w C a l c u l a t i o n   ( v e c 4   f r a g P o s L i g h t S p a c e )   { 
         / / � � � � � � � � � � 
         v e c 3   p r o j C o o r d s   =   f r a g P o s L i g h t S p a c e . x y z   /   f r a g P o s L i g h t S p a c e . w ; 
         / / � � � � � � [ 0 , 1 ] � � � � 
         p r o j C o o r d s   =   ( p r o j C o o r d s   +   1 )   *   0 . 5 ; 
         / / � � � � � � � � � � 
         f l o a t   c l o s e s t D e p t h   =   t e x t u r e ( s h a d o w M a p ,   p r o j C o o r d s . x y ) . r ; 
         / / � � � � � � � � � � 
         f l o a t   c u r r e n t D e p t h   =   p r o j C o o r d s . z ; 
         / / � � � � � � � � � � � � � � � � � � � � � � � � � � � � 
         f l o a t   s h a d o w   =   c u r r e n t D e p t h   >   c l o s e s t D e p t h ?   1 . 0   :   0 . 0 ; 
 
         r e t u r n   s h a d o w ; 
 } 
 
 
 v o i d   m a i n ( ) 
 { 
         / /f>y:m�^�V� 
         f l o a t   d e p t h V a l u e   =   t e x t u r e ( s h a d o w M a p ,   t e x c . x y ) . r ; 
         o u t p u t C o l o r   =   v e c 4 ( v e c 3 ( d e p t h V a l u e ) ,   1 . 0 ) ; 
         r e t u r n ; 
 
 
         / /f>y:w@�r 
 	 v e c 3   c o l o r   =   t e x t u r e ( d i f f u s e C o l o r ,   t e x c . x y ) . r g b ; 
         v e c 3   n o r m a l   =   n o r m a l i z e ( n o r m a l ) ; 
         v e c 3   l i g h t C o l o r   =   l i g h t . l i g h t C o l o r . x y z ; 
 
         v e c 3   a m b i e n t   =   0 . 0 5   *   c o l o r ; 
 
         / / v e c 3   l i g h t D i r   =   n o r m a l i z e ( ( l i g h t . l i g h t P o s   -   v W o r l d P o s ) . x y z ) ; 
 	 v e c 3   l i g h t D i r   =   n o r m a l i z e ( l i g h t . l i g h t D i r . x y z ) ; 
 	 l i g h t D i r   =   - l i g h t D i r ;   / / � � � � � � � � � � � � � � � � 
         f l o a t   d i f f   =   m a x ( d o t ( l i g h t D i r ,   n o r m a l ) ,   0 . 0 ) ; 
         v e c 3   d i f f u s e   =   d i f f   *   l i g h t C o l o r ; 
         
         f l o a t   s p e c   =   0 . 0 ; 
       / *   v e c 3   v i e w D i r   =   n o r m a l i z e ( v i e w P o s   -   v W o r l d P o s ) ; 
         v e c 3   h a l f w a y D i r   =   n o r m a l i z e ( l i g h t D i r   +   v i e w D i r ) ;     
         s p e c   =   p o w ( m a x ( d o t ( n o r m a l ,   h a l f w a y D i r ) ,   0 . 0 ) ,   6 4 . 0 ) ; * / 
         v e c 3   s p e c u l a r   =   s p e c   *   l i g h t C o l o r ;   
         / /   � � � � � � � � 
 	 f l o a t   s h a d o w   =     S h a d o w C a l c u l a t i o n ( F r a g P o s L i g h t S p a c e ) ; 
         v e c 3   l i g h t i n g   =   ( a m b i e n t   +   ( 1 . 0   -   s h a d o w )   *   ( d i f f u s e   +   s p e c u l a r ) )   *   c o l o r ;         
         
         o u t p u t C o l o r   =   v e c 4 ( l i g h t i n g ,   1 . 0 ) ; 
 
 
 
 	 / /   d i s c a r d   d i f f u s e   c o l o r 
 	 / * f l o a t   a m b i e n t S t r e n g t h   =   0 . 1 f ; 
         v e c 4   a m b i e n t   =   a m b i e n t S t r e n g t h   *   l i g h t . l i g h t C o l o r ; 
 
 	 v e c 4   d i f f   =   t e x t u r e ( d i f f u s e C o l o r ,   t e x c . x y ) ; 
 
 	 v e c 3   n o r m   =   n o r m a l i z e ( n o r m a l ) ; 
 	 v e c 3   d i r   =   n o r m a l i z e ( l i g h t . l i g h t D i r . x y z ) ; 
 	 f l o a t   d i f f F a c t o r   =   m a x ( d o t ( n o r m ,   d i r ) ,   0 . 0 ) ; 
 	 v e c 4   d i f f u s e   =   d i f f F a c t o r   *   l i g h t . l i g h t C o l o r ; 
 
 	 f l o a t   d e p t h   =   t e x t u r e (   s h a d o w M a p ,   v W o r l d P o s . x y   ) . z ; 
 
 	 o u t p u t C o l o r   =   d i f f   *   ( d i f f u s e   +   a m b i e n t ) ; * / 
 }   
 
