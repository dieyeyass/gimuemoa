<?php
if(isset ($_POST['valider'])){
// première étape : désactiver le cache lors de la phase de test
ini_set("soap.wsdl_cache_enabled", "0");
// lier le client au fichier WSDL
$clientSOAP = new SoapClient('../gimonline_mpi.wsdl');
include_once("concli.inc.php");
$certificat="";
$siteid="";
$requete=$connexion->query('SELECT siteid,certificat from marchand');

 while ($donnee = $requete->fetch())
            {   $certificat=$donnee['certificat']; 
                $siteid=$donnee['siteid']; 		
		    }
$pass_phrase= "TEST"."PAYMENT"."".$_POST['panier'].$_POST['amount'].$siteid.$_POST['currency'].$certificat;
$pass_signature= sha1($pass_phrase);
//VARIABLE A ENVOYER AU SERVEUR VIA LE SERVICE WEB
$gol_site_id=$siteid;
$gol_amount=$_POST['amount'];
$gol_currency=$_POST['currency'];
$gol_NumerosPanier=$_POST['panier'];
$gol_ctx_mode=$_POST['mode'];
$gol_page_action=$_POST['page_action'];
$gol_trans_date=date('d-m-Y');
$gol_Signature=$pass_signature;
//RECUPERATION DU TOKEN DE TRANSACTION
$token=$clientSOAP->makePaymentFunction($gol_site_id,$gol_amount,$gol_currency,$gol_NumerosPanier,$gol_ctx_mode,$gol_page_action,$gol_trans_date,$gol_Signature);
//TRAITEMENT DU TOKEN
if($token!="5888"){
	header('location:../server/payment.php?token='.$token.'');
	}
}
else{
?>
<html>
 <head>
  <link rel="stylesheet" type="text/css" href="../css/bootstrap.min.css">
  <link rel="stylesheet" type="text/css" href="../css/shipping.css">
 </head>
 <body>
  <div id="fondpage">
   <div id="infopayment">
    <form method="POST" action="shipping.php">
     <fieldset>
	  <legend>RESUME PAIEMENT</legend>
       <div id="droite">
	      <img src="../images/shirt.jpg"/>
	   </div>
		  <div id="gauche">
		  <p> Tee-shirt pack 8 </p>
		  <p> Quantite:120 </p>
		  <p> Montant Total
		  <input name="amount" type="text" style="border:none" value="120000"/>FCFA </p>
		  <input name="currency" type="hidden" value="XOF"/>
		  <input name="panier" type="hidden" value="000010"/>
		  <input name="mode" type="hidden" value="TEST"/>
		  <input name="page_action" type="hidden" value="PAYMENT"/>
		  <input type="submit" value="Passer au paiement" name='valider' style="color:red"/>
      </div>
	 </fieldset>
    </form>
   </div>	
  </div>
 </body>
</html>
<?php } ?>