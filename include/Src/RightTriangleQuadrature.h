/*
Copyright (c) 2018, Fabian Prada and Michael Kazhdan
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/
#ifndef RIGHT_TRIANGLE_QUADRATURE
#define RIGHT_TRIANGLE_QUADRATURE

#include <Misha/Geometry.h>

template< unsigned int Samples >
struct SegmentIntegrator
{
	static const double Weights[];
	static const double Positions[];
};
template<> const double SegmentIntegrator<1>::Weights[] = { 1. };
template<> const double SegmentIntegrator<2>::Weights[] = { 0.5 , 0.5 };
template<> const double SegmentIntegrator<3>::Weights[] = { 5./18 , 8./18 , 5./18 };
template<> const double SegmentIntegrator<4>::Weights[] = { ( 18.-sqrt(30) ) / 72 , ( 18.+sqrt(30) ) / 72 , ( 18.+sqrt(30) ) / 72 , ( 18.-sqrt(30) ) / 72 };
template<> const double SegmentIntegrator<5>::Weights[] = { ( 322. - 13.*sqrt(70.) )/900. , ( 322. + 13.*sqrt(70.) )/900. , 128./225 , ( 322. + 13.*sqrt(70.) )/900. , ( 322. - 13.*sqrt(70.) )/900. };
template<> const double SegmentIntegrator<1>::Positions[] = {                      1./2                      };
template<> const double SegmentIntegrator<2>::Positions[] = { 1./2 - sqrt(1./12) ,        1./2 + sqrt(1./12) };
template<> const double SegmentIntegrator<3>::Positions[] = { 1./2 - sqrt(3./20) , 1./2 , 1./2 + sqrt(3./20) };
template<> const double SegmentIntegrator<4>::Positions[] = { 1./2 - sqrt( 3./28 + 2./28 * sqrt(6./5) ) , 1./2 - sqrt( 3./28 - 2./28 * sqrt(6./5) ) , 1./2 + sqrt( 3./28 - 2./28 * sqrt(6./5) ) , 1./2 + sqrt( 3./28 + 2./28 * sqrt(6./5) ) };
template<> const double SegmentIntegrator<5>::Positions[] = { 1./2 - 1./6 * sqrt( 5. + 2. * sqrt(10./7) ) , 1./2 - 1./6 * sqrt( 5. - 2. * sqrt(10./7) ) , 1./2 , 1./2 + 1./6 * sqrt( 5. - 2. * sqrt(10./7) ) , 1./2 + 1./6 * sqrt( 5. + 2. * sqrt(10./7) ) };

template< unsigned int Samples >
struct QuadIntegrator
{
	QuadIntegrator( const Point2D< double > corners[2][2] )
	{
		// P(x,y) = c(0,0)*(1-x)*(1-y) + c(0,1)*(1-x)*y + c(1,1)*x*y + c(1,0)*x*(1-y)
		// dP(x,y) = [ -c(0,0)*(1-y) - c(0,1)*y + c(1,1)*y + c(1,0)*(1-y) , -c(0,0)*(1-x) + c(0,1)*(1-x) + c(1,1)*x  - c(1,0)*x ]
		for( int i=0 ; i<Samples ; i++ ) for( int j=0 ; j<Samples ; j++ )
		{
			const double x = SegmentIntegrator< Samples >::Positions[i] , y = SegmentIntegrator< Samples >::Positions[j];
			const double d[2][2] =
			{
				{ ( corners[1][0][0] - corners[0][0][0] )*(1.-y) + ( corners[1][1][0] - corners[0][1][0] )*y , ( corners[1][0][1] - corners[0][0][1] )*(1.-y) + ( corners[1][1][1] - corners[0][1][1] )*y } ,
				{ ( corners[0][1][0] - corners[0][0][0] )*(1.-x) + ( corners[1][1][0] - corners[1][0][0] )*x , ( corners[0][1][1] - corners[0][0][1] )*(1.-x) + ( corners[1][1][1] - corners[1][0][1] )*x }
			};

			positions[i*Samples+j] = corners[0][0] * (1.-x) * (1.-y) + corners[0][1] * (1.-x) * (   y) + corners[1][1] * (   x) * (   y) + corners[1][0] * (   x) * (1.-y) ;
			weights[i*Samples+j] = SegmentIntegrator< Samples >::Weights[i] * SegmentIntegrator< Samples >::Weights[j] * ( d[0][0] * d[1][1] - d[0][1] * d[1][0] );
		}
	};
	double weights[Samples*Samples];
	Point2D< double > positions[Samples*Samples];
};

template< unsigned int Samples >
struct TriangleIntegrator
{
	static const double Weights[];
	static const Point2D< double > Positions[];
};

// http://www.cs.rpi.edu/~flaherje/pdf/fea6.pdf
template<> const double TriangleIntegrator<1>::Weights[] =
{
	1.
};
template<> const Point2D< double > TriangleIntegrator<1>::Positions[] =
{
	Point2D< double >( 1./3 , 1./3 )
};

template<> const double TriangleIntegrator<3>::Weights[] =
{
	1./3 ,
	1./3 ,
	1./3
};
template<> const Point2D< double > TriangleIntegrator<3>::Positions[] =
{
	Point2D< double >( 1./6 , 1./6 ) ,
	Point2D< double >( 1./6 , 2./3 ) ,
	Point2D< double >( 2./3 , 1./6 )
};

template<> const double TriangleIntegrator<6>::Weights[] =
{
	0.109951743655322 ,
	0.109951743655322 ,
	0.109951743655322 ,
	0.223381589678011 ,
	0.223381589678011 ,
	0.223381589678011
};
template<> const Point2D< double > TriangleIntegrator<6>::Positions[] =
{
	Point2D< double >( 0.816847572980459  , 0.0915762135097705 ),
	Point2D< double >( 0.0915762135097705 , 0.816847572980459  ),
	Point2D< double >( 0.0915762135097705 , 0.0915762135097705 ),
	Point2D< double >( 0.108103018168070  , 0.445948490915965  ),
	Point2D< double >( 0.445948490915965  , 0.108103018168070  ),
	Point2D< double >( 0.445948490915965  , 0.445948490915965  )
};

template<> const double TriangleIntegrator<12>::Weights[] =
{
	0.116786275726379 ,
	0.116786275726379 ,
	0.116786275726379 ,
	0.050844906370207 ,
	0.050844906370207 ,
	0.050844906370207 ,
	0.082851075618374 ,
	0.082851075618374 ,
	0.082851075618374 ,
	0.082851075618374 ,
	0.082851075618374 ,
	0.082851075618374
};
template<> const Point2D< double > TriangleIntegrator<12>::Positions[] =
{
	Point2D< double >( 0.249286745170910 , 0.249286745170910 ) ,
	Point2D< double >( 0.249286745170910 , 0.501426509658179 ) ,
	Point2D< double >( 0.501426509658179 , 0.249286745170910 ) ,
	Point2D< double >( 0.063089014491502 , 0.063089014491502 ) ,
	Point2D< double >( 0.063089014491502 , 0.873821971016996 ) ,
	Point2D< double >( 0.873821971016996 , 0.063089014491502 ) ,
	Point2D< double >( 0.310352451033785 , 0.636502499121399 ) ,
	Point2D< double >( 0.636502499121399 , 0.053145049844816 ) ,
	Point2D< double >( 0.053145049844816 , 0.310352451033785 ) ,
	Point2D< double >( 0.636502499121399 , 0.310352451033785 ) ,
	Point2D< double >( 0.310352451033785 , 0.053145049844816 ) ,
	Point2D< double >( 0.053145049844816 , 0.636502499121399 )
};

// [Taylor, 2007] Asymmetric cubature formulas for polynomial integration in the triangle and square
template<> const double TriangleIntegrator<24>::Weights[] =
{
	1.7344807725532943e-01 / 2. ,
	1.9053311454269983e-01 / 2. ,
	1.6882888511942015e-01 / 2. ,
	1.0546076281767805e-01 / 2. ,
	1.4815929467355968e-01 / 2. ,
	1.0983120878770872e-01 / 2. ,
	1.0507331820482332e-01 / 2. ,
	8.5924658784158670e-02 / 2. ,
	1.2537585060182724e-01 / 2. ,
	1.1594828119739846e-01 / 2. ,
	1.3237226895051976e-01 / 2. ,
	1.2348449173239080e-01 / 2. ,
	2.9216658446243379e-02 / 2. ,
	6.4605204046914597e-02 / 2. ,
	3.9118824435043810e-02 / 2. ,
	2.2133893564494179e-02 / 2. ,
	3.0406188052025412e-02 / 2. ,
	2.1333382551825181e-02 / 2. ,
	2.3800609628471206e-02 / 2. ,
	2.9693247293360987e-02 / 2. ,
	3.5311689185924387e-02 / 2. ,
	2.6798161571713618e-02 / 2. ,
	3.0312523835131357e-02 / 2. ,
	6.2829404721337689e-02 / 2.
};
template<> const Point2D< double > TriangleIntegrator<24>::Positions[] =
{
	Point2D< double >( 5.0550507373529086e-01 , 2.0776116575484826e-01 ) ,
	Point2D< double >( 2.7542385024412980e-01 , 4.8123289062464247e-01 ) ,
	Point2D< double >( 2.6481531651496770e-01 , 2.7586334089315967e-01 ) ,
	Point2D< double >( 7.5329402776254240e-01 , 1.0954959855585467e-01 ) ,
	Point2D< double >( 5.2433682558924433e-01 , 3.6419744430339263e-01 ) ,
	Point2D< double >( 2.9530445535851102e-01 , 6.4203365318662664e-01 ) ,
	Point2D< double >( 1.0614642990289996e-01 , 7.6777680170023954e-01 ) ,
	Point2D< double >( 6.3491832379200652e-01 , 3.6036266787907723e-02 ) ,
	Point2D< double >( 3.8729657913960353e-01 , 8.4198522115543739e-02 ) ,
	Point2D< double >( 1.6929927488966462e-01 , 1.0999439055630450e-01 ) ,
	Point2D< double >( 8.0491894656105567e-02 , 5.7966325105486349e-01 ) ,
	Point2D< double >( 9.5379208487721689e-02 , 3.3947290311800554e-01 ) ,
	Point2D< double >( 9.2899486985787905e-01 , 4.7768381772022417e-02 ) ,
	Point2D< double >( 7.4726591728868819e-01 , 2.2376358774275851e-01 ) ,
	Point2D< double >( 5.0365825075943971e-01 , 4.8798437805397499e-01 ) ,
	Point2D< double >( 1.6134650499890957e-01 , 8.3865349500109043e-01 ) ,
	Point2D< double >( 2.9553592846822851e-02 , 9.3049846900263089e-01 ) ,
	Point2D< double >( 8.6854386943076545e-01 , 3.8102570854643414e-03 ) ,
	Point2D< double >( 3.9366774470722010e-01 , 0.0000000000000000e+00 ) ,
	Point2D< double >( 1.7690730625559031e-01 , 1.0939142057119933e-02 ) ,
	Point2D< double >( 3.5319656252586096e-02 , 3.9099745550423282e-02 ) ,
	Point2D< double >( 0.0000000000000000e+00 , 7.7757518429429107e-01 ) ,
	Point2D< double >( 0.0000000000000000e+00 , 4.6374383867430541e-01 ) ,
	Point2D< double >( 3.0573404093099332e-02 , 1.9305903224251936e-01 )
};

template<> const double TriangleIntegrator<32>::Weights[] =
{
	1.1887566790227083e-01 / 2. ,
	1.5044412520664885e-01 / 2. ,
	1.2632909284531338e-01 / 2. ,
	1.0192984975357525e-01 / 2. ,
	9.4999150650614317e-02 / 2. ,
	4.4981492398316447e-02 / 2. ,
	7.9147211585943858e-02 / 2. ,
	1.1997941465421234e-01 / 2. ,
	1.0670416609764186e-01 / 2. ,
	6.1058344824144795e-02 / 2. ,
	8.2563774790925248e-02 / 2. ,
	9.6297610073814668e-02 / 2. ,
	9.1875684331583440e-02 / 2. ,
	6.1150555208077911e-02 / 2. ,
	4.3370170834023010e-02 / 2. ,
	1.0829374522633514e-01 / 2. ,
	5.5887468639759713e-02 / 2. ,
	1.3351800054734712e-02 / 2. ,
	1.5428984747249670e-02 / 2. ,
	5.0198346855370224e-02 / 2. ,
	5.6291117210426664e-02 / 2. ,
	4.1240008239364231e-02 / 2. ,
	1.4239502872161450e-02 / 2. ,
	1.3691069308687381e-02 / 2. ,
	1.9309417484872689e-02 / 2. ,
	3.7090960843213061e-02 / 2. ,
	3.6967371622461546e-02 / 2. ,
	2.1018653471205032e-02 / 2. ,
	9.7760996293200769e-03 / 2. ,
	5.6339308919459923e-02 / 2. ,
	4.9808146403015403e-02 / 2. ,
	2.1361687315256585e-02 / 2. 
};
template<> const Point2D< double > TriangleIntegrator<32>::Positions[] =
{
	Point2D< double >( 3.7986021093401956e-01 , 2.1078525939140391e-01 ) ,
	Point2D< double >( 3.0141709320909305e-01 , 4.0978657777002531e-01 ) ,
	Point2D< double >( 5.5802528953120256e-01 , 2.1377743253005960e-01 ) ,
	Point2D< double >( 1.2512299505810387e-01 , 6.1938125736255578e-01 ) ,
	Point2D< double >( 2.1117939909804934e-01 , 2.4498296509349016e-01 ) ,
	Point2D< double >( 8.5431474947580432e-01 , 7.1871496101589105e-02 ) ,
	Point2D< double >( 7.1788185898052326e-01 , 2.0376848107772977e-01 ) ,
	Point2D< double >( 4.6631787462323071e-01 , 4.0896380449124475e-01 ) ,
	Point2D< double >( 2.5015500335339214e-01 , 6.2768261568031403e-01 ) ,
	Point2D< double >( 7.9955384841381316e-02 , 8.2600331401756000e-01 ) ,
	Point2D< double >( 7.1008125956836521e-01 , 6.4413220382260550e-02 ) ,
	Point2D< double >( 4.9732063377796598e-01 , 7.0566724344036824e-02 ) ,
	Point2D< double >( 2.6077068256562896e-01 , 9.5428585810584610e-02 ) ,
	Point2D< double >( 8.9602705800587434e-02 , 1.1638649906727733e-01 ) ,
	Point2D< double >( 2.3088148766115757e-02 , 7.4918973979067949e-01 ) ,
	Point2D< double >( 1.2953296900433620e-01 , 4.2260565743346001e-01 ) ,
	Point2D< double >( 9.3448087604440955e-02 , 2.4345813394879973e-01 ) ,
	Point2D< double >( 9.5526919357006035e-01 , 2.3551733249578710e-02 ) ,
	Point2D< double >( 8.4593539837314391e-01 , 1.5406460162685609e-01 ) ,
	Point2D< double >( 6.1600929617267497e-01 , 3.6118159118967208e-01 ) ,
	Point2D< double >( 3.9316510319604808e-01 , 5.8168921474014745e-01 ) ,
	Point2D< double >( 1.8920633061715936e-01 , 7.8860171922313160e-01 ) ,
	Point2D< double >( 4.3010560106405471e-02 , 9.4547507322097091e-01 ) ,
	Point2D< double >( 8.5815888421533082e-01 , 0.0000000000000000e+00 ) ,
	Point2D< double >( 6.2731531923241179e-01 , 0.0000000000000000e+00 ) ,
	Point2D< double >( 3.6384660446077510e-01 , 1.4566514788346974e-02 ) ,
	Point2D< double >( 1.5557066896897953e-01 , 2.1152223383121949e-02 ) ,
	Point2D< double >( 2.9754117496841759e-02 , 2.7110971356255786e-02 ) ,
	Point2D< double >( 0.0000000000000000e+00 , 9.2734897448394982e-01 ) ,
	Point2D< double >( 2.5716283623693881e-02 , 5.4444667627192522e-01 ) ,
	Point2D< double >( 2.4506286636990005e-02 , 3.3212908394764507e-01 ) ,
	Point2D< double >( 9.2296909059649268e-03 , 1.4604496167217568e-01 )
};

#endif// RIGHT_TRIANGLE_QUADRATURE
