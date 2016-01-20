#!/usr/bin/perl

use strict;
use warnings;
use List::MoreUtils qw(any);

sub massert ($ $) {
	my ( $condition, $message ) = ( $_[0], $_[1] );

	if ( !$condition ) {
		exit( print($message) );
	}
}

sub create_structure_list ($) {
	my ($file) = ( $_[0] );
	open( INPUT, "<$file" ) or die "Could not open '$file' $!\n";

	my %hashtable;

	while ( my $line = <INPUT> ) {
		chomp $line;
		my @fields = split "\t", $line;

		my $structure = $fields[0];
		
		if($hashtable{$structure}) {
			$hashtable{$structure} += 1;
			next;
		}
		
		$hashtable{$structure} = 1;
	}
	return keys %hashtable;
}

sub create_hashtable_graph ($) {
	my ($file) = ( $_[0] );
	open( INPUT, "<$file" ) or die "Could not open '$file' $!\n";

	my %hashtable;

	while ( my $line = <INPUT> ) {
		chomp $line;
		my @fields = split "\t", $line;

		my $structure1 = $fields[0];
		my $structure2 = $fields[1];
		my $similarity = $fields[2];

		$hashtable{"$structure1$structure2"} = $similarity;
	}

	return %hashtable;
}

sub create_hashtable_cluster ($) {
	my ($file) = ( $_[0] );
	open( INPUT, "<$file" ) or die "Could not open '$file' $!\n";

	my %hashtable;

	while ( my $line = <INPUT> ) {
		chomp $line;
		my @fields = split "\t", $line;

		my $cluster   = $fields[0];
		my $structure = $fields[2];

		$hashtable{$structure} = $cluster;
	}

	return %hashtable;
}

sub main ($ $ $ $) {
	my ( $cluster_aacid, $cluster_salami, $graph_aacid, $graph_salami ) =
	  ( $_[0], $_[1], $_[2], $_[3] );

	my @structures = create_structure_list($graph_aacid);

	my %hashtable_cluster_aacid  = create_hashtable_cluster($cluster_aacid);
	my %hashtable_cluster_salami = create_hashtable_cluster($cluster_salami);

	my %hashtable_graph_aacid  = create_hashtable_graph($graph_aacid);
	my %hashtable_graph_salami = create_hashtable_graph($graph_salami);

	print("structure a;structure b;tm_score aacid;tm_score salami\n");

	for my $structure1 (@structures) {
		if ($hashtable_cluster_aacid{$structure1} and $hashtable_cluster_salami{$structure1} ) {
			for my $structure2 (@structures) {
				if ($hashtable_cluster_aacid{$structure2} and $hashtable_cluster_salami{$structure2} ) {
					
					my $structure1_cluster_aacid = $hashtable_cluster_aacid{$structure1};
					my $structure2_cluster_aacid = $hashtable_cluster_aacid{$structure2};
					my $structure1_cluster_salami = $hashtable_cluster_salami{$structure1};
					my $structure2_cluster_salami = $hashtable_cluster_salami{$structure2};

					if ($structure1_cluster_aacid == $structure2_cluster_aacid ) {
						if ( $structure1_cluster_salami == $structure2_cluster_salami ) {


							my $hash1 = "$structure1$structure2";
							if ( not $hashtable_graph_aacid{$hash1} ) {
								$hash1 = "$structure2$structure1";
							}

							my $hash2 = "$structure1$structure2";
							if ( not $hashtable_graph_salami{$hash2} ) {
								$hash2 = "$structure2$structure1";
							}
								
							if (not $hashtable_graph_aacid{$hash1} 
								or not $hashtable_graph_salami{$hash2}) {
								next;
							}

							my $similarity_aacid  = $hashtable_graph_aacid{$hash1};
							my $similarity_salami = $hashtable_graph_salami{$hash2};

							print("$structure1;$structure2;$similarity_aacid;$similarity_salami\n");
						}
					}
				}
			}
		}
	}
}

$ARGV[0] = "var/graphtocluster_aacid.clust";
$ARGV[1] = "var/graphtocluster_salami.clust";
$ARGV[2] = "var/wurstimibiss_aacid.ncol";
$ARGV[3] = "var/wurstimibiss_salami.ncol";

massert( defined( $ARGV[0] ), "Cluster aacid file can not be empty" );
massert( defined( $ARGV[1] ), "Cluster salami file can not be empty" );
massert( defined( $ARGV[2] ), "Graph aacid file can not be empty" );
massert( defined( $ARGV[3] ), "Graph salami file can not be empty" );

exit( main( $ARGV[0], $ARGV[1], $ARGV[2], $ARGV[3] ) );
